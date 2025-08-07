#include "rendering/TileGrid.h"
#include "rendering/Tile.h"
#include "rendering/BitmapPool.h"
#include "rendering/TilePool.h"

#include <Autolock.h>
#include <Bitmap.h>

#include "WebView.h"
#include "WebPage.h"

TileGrid::TileGrid(BWebPage* webPage, ThreadPool* threadPool, int32 tileSize, size_t softLimit, size_t hardLimit)
    :
    fGridLock("TileGridLock"),
    fDirtyTilesLock("TileGridDirtyTilesLock"),
    fCurrentMemoryUsage(0),
    fSoftMemoryLimit(softLimit),
    fHardMemoryLimit(hardLimit),
    fWebPage(webPage),
    fThreadPool(threadPool),
    fTileSize(tileSize)
{
}

TileGrid::~TileGrid()
{
}

Tile*
TileGrid::GetTile(const TileIndex& index)
{
    BAutolock locker(fGridLock);
    auto it = fGrid.find(index);
    if (it != fGrid.end()) {
        _PromoteTile(index);
        return it->second.get();
    }
    return nullptr;
}

Tile*
TileGrid::GetOrCreateTile(const TileIndex& index)
{
    BAutolock locker(fGridLock);
    auto it = fGrid.find(index);
    if (it != fGrid.end()) {
        _PromoteTile(index);
        return it->second.get();
    }

    auto newTile = TilePool::GetInstance().Acquire(index.col, index.row);
    Tile* tilePtr = newTile.get();
    fGrid[index] = std::move(newTile);

    // SIEVE1: On first access, add to candidates, not the main cache.
    fSieveCandidates.insert(index);

    return tilePtr;
}

void
TileGrid::SetMap(std::unordered_map<TileIndex, std::unique_ptr<Tile>>&& map)
{
    BAutolock locker(fGridLock);
    fGrid = std::move(map);
}

void
TileGrid::RemoveTile(const TileIndex& index)
{
    BAutolock locker(fGridLock);
    fSieveCandidates.erase(index);

    auto it = fGrid.find(index);
    if (it == fGrid.end())
        return;

    // Update memory usage before destroying the tile.
    if (it->second->GetBitmap())
        fCurrentMemoryUsage -= it->second->GetBitmap()->Size();
    else if (!it->second->GetCompressedData().empty())
        fCurrentMemoryUsage -= it->second->GetCompressedData().size();

    TilePool::GetInstance().Release(std::move(it->second));

    // Remove from LRU lists and maps.
    auto renderedIt = fRenderedLruMap.find(index);
    if (renderedIt != fRenderedLruMap.end()) {
        fRenderedLruQueue.erase(renderedIt->second);
        fRenderedLruMap.erase(renderedIt);
    }

    auto compressedIt = fCompressedLruMap.find(index);
    if (compressedIt != fCompressedLruMap.end()) {
        fCompressedLruQueue.erase(compressedIt->second);
        fCompressedLruMap.erase(compressedIt);
    }

    fGrid.erase(it);
}

void
TileGrid::UpdateMemoryUsage(int64_t delta)
{
    BAutolock locker(fGridLock);
    fCurrentMemoryUsage += delta;
}

void
TileGrid::SetMemoryLimits(size_t softLimit, size_t hardLimit)
{
    BAutolock locker(fGridLock);
    fSoftMemoryLimit = softLimit;
    fHardMemoryLimit = hardLimit;
    EvictTiles(true);
}

void
TileGrid::EvictTiles(bool aggressive)
{
    BAutolock locker(fGridLock);
    size_t limit = aggressive ? fHardMemoryLimit : fSoftMemoryLimit;

    while (fCurrentMemoryUsage > limit && (!fCompressedLruQueue.empty() || !fRenderedLruQueue.empty())) {
        if (!fCompressedLruQueue.empty()) {
            TileIndex toEvict = fCompressedLruQueue.back();
            fCompressedLruQueue.pop_back();
            fCompressedLruMap.erase(toEvict);

            auto it = fGrid.find(toEvict);
            if (it != fGrid.end()) {
                Tile* tile = it->second.get();
                if (tile->GetState() == COMPRESSED) {
                    fCurrentMemoryUsage -= tile->GetCompressedData().size();
                    tile->SetCompressedData({});
                    tile->SetState(NEEDS_RENDER);
                }
            }
        } else {
            const int kCandidateCount = 16;
            TileIndex worstCandidate;
            double maxScore = -1.0;
            auto candidateIt = fRenderedLruQueue.rbegin();

            for (int i = 0; i < kCandidateCount && candidateIt != fRenderedLruQueue.rend(); ++i, ++candidateIt) {
                const TileIndex& currentIndex = *candidateIt;
                auto gridIt = fGrid.find(currentIndex);
                if (gridIt == fGrid.end())
                    continue;

                Tile* tile = gridIt->second.get();
                if (tile->IsPinned() || (system_time() - tile->LastEvictionTime() < 1000000))
                    continue;

                double age = (double)(system_time() - tile->LastAccessTime());
                double complexity = (double)tile->GetRenderComplexity();
                double importance = (double)tile->GetFrameImportance();
                double memUsage = (double)(tile->GetBitmap() ? tile->GetBitmap()->Size() : 1);

                // Higher score is worse (more likely to be evicted).
                double score = (age * memUsage) / (complexity * importance);

                if (score > maxScore) {
                    maxScore = score;
                    worstCandidate = currentIndex;
                }
            }

            if (maxScore >= 0) {
                // We found a victim, evict it.
                auto mapIt = fRenderedLruMap.find(worstCandidate);
                if (mapIt != fRenderedLruMap.end()) {
                    fRenderedLruQueue.erase(mapIt->second);
                    fRenderedLruMap.erase(mapIt);
                }

                auto gridIt = fGrid.find(worstCandidate);
                if (gridIt != fGrid.end()) {
                    Tile* tile = gridIt->second.get();
                    if (tile->GetState() == RENDERED && tile->GetBitmap()) {
                        fCurrentMemoryUsage -= tile->GetBitmap()->Size();
                        BitmapPool::GetInstance().Release(tile->TakeBitmap());
                        tile->SetState(NEEDS_RENDER);
                        tile->SetLastEvictionTime(system_time());
                    }
                }
            } else {
                // No suitable candidate found in the top N, so stop.
                break;
            }
        }
    }
}

void
TileGrid::_PromoteTile(const TileIndex& index)
{
    auto gridIt = fGrid.find(index);
    if (gridIt == fGrid.end())
        return;

    Tile* tile = gridIt->second.get();
    tile->IncrementAccessCount();
    tile->SetLastAccessTime(system_time());

    // If the tile is in the rendered LRU queue, move it to the front.
    auto it = fRenderedLruMap.find(index);
    if (it != fRenderedLruMap.end()) {
        fRenderedLruQueue.erase(it->second);
        fRenderedLruQueue.push_front(index);
        fRenderedLruMap[index] = fRenderedLruQueue.begin();
        return;
    }

    // If the tile is in the compressed LRU queue, move it to the front.
    it = fCompressedLruMap.find(index);
    if (it != fCompressedLruMap.end()) {
        fCompressedLruQueue.erase(it->second);
        fCompressedLruQueue.push_front(index);
        fCompressedLruMap[index] = fCompressedLruQueue.begin();
        return;
    }

    // SIEVE1: If it's a candidate, promote it to the main cache.
    if (fSieveCandidates.count(index)) {
        fSieveCandidates.erase(index);
        fRenderedLruQueue.push_front(index);
        fRenderedLruMap[index] = fRenderedLruQueue.begin();
    }


BRect
TileGrid::Scroll(int xOffset, int yOffset, const BRect& rectToScroll)
{
    // This function completely rebuilds the grid and LRU lists
    // to reflect the scrolled state.

    std::unordered_map<TileIndex, std::unique_ptr<Tile>> newGrid;
    std::list<TileIndex> newRenderedLruQueue;
    std::unordered_map<TileIndex, std::list<TileIndex>::iterator> newRenderedLruMap;
    std::list<TileIndex> newCompressedLruQueue;
    std::unordered_map<TileIndex, std::list<TileIndex>::iterator> newCompressedLruMap;
    std::unordered_set<TileIndex> newSieveCandidates;

    BRect invalidRect;

    // Process the old grid map. We move tiles from fGrid into newGrid.
    for (auto& pair : fGrid) {
        const TileIndex& oldIndex = pair.first;
        std::unique_ptr<Tile>& tile = pair.second;

        BRect tileRect(tile->fX * fTileSize, tile->fY * fTileSize,
            (tile->fX + 1) * fTileSize - 1, (tile->fY + 1) * fTileSize - 1);
        BRect scrolledRect = tileRect.OffsetByCopy(xOffset, yOffset);

        if (rectToScroll.Intersects(scrolledRect)) {
            TileIndex newIndex = { (int)floor(scrolledRect.top / fTileSize),
                (int)floor(scrolledRect.left / fTileSize) };
            tile->fX = newIndex.col;
            tile->fY = newIndex.row;

            // Move the tile to the new data structures, preserving its cache status.
            if (fRenderedLruMap.count(oldIndex)) {
                newRenderedLruQueue.push_back(newIndex);
                newRenderedLruMap[newIndex] = std::prev(newRenderedLruQueue.end());
            } else if (fCompressedLruMap.count(oldIndex)) {
                newCompressedLruQueue.push_back(newIndex);
                newCompressedLruMap[newIndex] = std::prev(newCompressedLruQueue.end());
            } else if (fSieveCandidates.count(oldIndex)) {
                newSieveCandidates.insert(newIndex);
            }

            newGrid[newIndex] = std::move(tile);
        } else {
            invalidRect = invalidRect | tileRect;
            // This tile is scrolled out of view. Release it to the pool.
            TilePool::GetInstance().Release(std::move(tile));
        }
    }

    // Now, efficiently swap all the data structures.
    fGrid = std::move(newGrid);
    fRenderedLruQueue = std::move(newRenderedLruQueue);
    fRenderedLruMap = std::move(newRenderedLruMap);
    fCompressedLruQueue = std::move(newCompressedLruQueue);
    fCompressedLruMap = std::move(newCompressedLruMap);
    fSieveCandidates = std::move(newSieveCandidates);

    return invalidRect;
}


void
TileGrid::UpgradeCompressionTier()
{
    BAutolock locker(fGridLock);
    // Copy the queue to avoid iterator invalidation issues if a background
    // thread modifies the main queue while we are iterating.
    std::list<TileIndex> queueCopy = fCompressedLruQueue;
    locker.Unlock();

    for (const auto& index : queueCopy) {
        // GetTile also promotes the tile, which is a minor side-effect.
        Tile* tile = GetTile(index);
        if (!tile)
            continue;

        if (tile->Lock()) {
            if (tile->GetState() == COMPRESSED
                && tile->GetCompressionLevel() == COMPRESSED_FAST
                && (system_time() - tile->LastAccessTime() > 30 * 1000 * 1000)) {

                tile->SetState(COMPRESSING);
                fThreadPool->Enqueue([this, tile]() {
                    // This task now "owns" the tile lock until it's done.
                    tile->Recompress(this, COMPRESSED_HIGH);
                    tile->Unlock();
                });
            } else {
                tile->Unlock();
            }
        }
    }
}
}

void
TileGrid::MarkTileAsDirty(const TileIndex& index)
{
    BAutolock locker(fDirtyTilesLock);
    fDirtyTiles.insert(index);
}

void
TileGrid::ProcessDirtyTiles(int32& renderBudget)
{
    BAutolock locker(fDirtyTilesLock);
    if (fDirtyTiles.empty())
        return;

    std::unordered_set<TileIndex> dirtyTiles = std::move(fDirtyTiles);
    locker.Unlock();

    for (const auto& index : dirtyTiles) {
        Tile* tile = GetTile(index);
        if (!tile)
            continue;

        BAutolock tileLocker(tile->Locker());
        if (tile->GetState() == COMPRESSED) {
            tile->SetState(DECOMPRESSING);
            fThreadPool->Enqueue([this, tile, index]() {
                BAutolock tileLocker(tile->Locker());
                if (tile->Decompress(this)) {
                    MoveToRenderedQueue(index);
                } else {
                    tile->SetState(NEEDS_RENDER);
                    MarkTileAsDirty(index);
                }
            });
        } else if (tile->GetState() == NEEDS_RENDER || tile->GetState() == RENDERED) {
            if (renderBudget <= 0) {
                MarkTileAsDirty(index);
                continue;
            }
            renderBudget--;
            tile->SetState(RENDERING);
            fThreadPool->Enqueue([this, tile]() {
                fWebPage->_RenderTile(tile);
            });
        }
    }
}


void
TileGrid::PrefetchTiles(BRect viewport, BPoint scrollVelocity)
{
    const float kPrefetchFactor = 2.0;
    BRect prefetchRect = viewport;
    prefetchRect.OffsetBy(scrollVelocity.x * kPrefetchFactor, scrollVelocity.y * kPrefetchFactor);

    BAutolock locker(fGridLock);

    int32 first_col = floor(prefetchRect.left / fTileSize);
    int32 first_row = floor(prefetchRect.top / fTileSize);
    int32 last_col = floor(prefetchRect.right / fTileSize);
    int32 last_row = floor(prefetchRect.bottom / fTileSize);

    for (int32 r = first_row; r <= last_row; r++) {
        for (int32 c = last_col; c >= first_col; c--) {
            TileIndex index = {r, c};
            Tile* tile = GetOrCreateTile(index);
            if (!tile)
                continue;

            MarkTileAsDirty(index);
        }
    }
}

void
TileGrid::MoveToCompressedQueue(const TileIndex& index)
{
    BAutolock locker(fGridLock);
    auto it = fRenderedLruMap.find(index);
    if (it != fRenderedLruMap.end()) {
        fRenderedLruQueue.erase(it->second);
        fRenderedLruMap.erase(it);
        fCompressedLruQueue.push_front(index);
        fCompressedLruMap[index] = fCompressedLruQueue.begin();
    }
}

void
TileGrid::MoveToRenderedQueue(const TileIndex& index)
{
    BAutolock locker(fGridLock);
    auto it = fCompressedLruMap.find(index);
    if (it != fCompressedLruMap.end()) {
        fCompressedLruQueue.erase(it->second);
        fCompressedLruMap.erase(it);
        fRenderedLruQueue.push_front(index);
        fRenderedLruMap[index] = fRenderedLruQueue.begin();
    }
}
