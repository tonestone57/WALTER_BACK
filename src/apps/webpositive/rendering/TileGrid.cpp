#include "rendering/TileGrid.h"
#include "rendering/Tile.h"
#include "rendering/BitmapPool.h"
#include "rendering/RenderingConstants.h"

#include <Autolock.h>
#include <Bitmap.h>

#include "WebView.h"
#include "WebPage.h"

TileGrid::TileGrid(BWebPage* webPage, ThreadPool* threadPool, size_t softLimit, size_t hardLimit)
    :
    fGridLock("TileGridLock"),
    fCurrentMemoryUsage(0),
    fSoftMemoryLimit(softLimit),
    fHardMemoryLimit(hardLimit),
    fWebPage(webPage),
    fThreadPool(threadPool)
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

    auto newTile = std::make_unique<Tile>(index.col, index.row);
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
            TileIndex toEvict = fRenderedLruQueue.back();
            fRenderedLruQueue.pop_back();
            fRenderedLruMap.erase(toEvict);

            auto it = fGrid.find(toEvict);
            if (it != fGrid.end()) {
                Tile* tile = it->second.get();
                if (tile->IsPinned() || (system_time() - tile->LastEvictionTime() < 1000000)) {
                    fRenderedLruQueue.push_front(toEvict);
                    fRenderedLruMap[toEvict] = fRenderedLruQueue.begin();
                    continue;
                }
                if (tile->GetState() == RENDERED && tile->GetBitmap()) {
                    fCurrentMemoryUsage -= tile->GetBitmap()->Size();
                    BitmapPool::GetInstance().Release(tile->TakeBitmap());
                    tile->SetState(NEEDS_RENDER);
                    tile->SetLastEvictionTime(system_time());
                }
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
}


void
TileGrid::PrefetchTiles(BRect viewport, BPoint scrollVelocity)
{
    const float kPrefetchFactor = 2.0;
    BRect prefetchRect = viewport;
    prefetchRect.OffsetBy(scrollVelocity.x * kPrefetchFactor, scrollVelocity.y * kPrefetchFactor);

    BAutolock locker(fGridLock);

    int32 first_col = floor(prefetchRect.left / kTileSize);
    int32 first_row = floor(prefetchRect.top / kTileSize);
    int32 last_col = floor(prefetchRect.right / kTileSize);
    int32 last_row = floor(prefetchRect.bottom / kTileSize);

    for (int32 r = first_row; r <= last_row; r++) {
        for (int32 c = last_col; c >= first_col; c--) {
            TileIndex index = {r, c};
            Tile* tile = GetOrCreateTile(index);
            if (!tile)
                continue;

            BAutolock tileLocker(tile->Locker());
            if (tile->GetState() == COMPRESSED) {
                tile->SetState(DECOMPRESSING);
                fThreadPool->Enqueue([this, tile]() {
                    BAutolock tileLocker(tile->Locker());
                    if (tile->Decompress(this)) {
                        MoveToRenderedQueue(TileIndex{tile->GetY(), tile->GetX()});
                    } else {
                        tile->SetState(NEEDS_RENDER);
                    }
                });
            } else if (tile->GetState() == NEEDS_RENDER) {
                fWebPage->paint(BRect(c * kTileSize, r * kTileSize, (c + 1) * kTileSize -1, (r + 1) * kTileSize -1), false);
            }
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
