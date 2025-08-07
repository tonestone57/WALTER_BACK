#include "rendering/TileGrid.h"
#include "rendering/Tile.h"
#include "rendering/BitmapPool.h"

#include <Autolock.h>
#include <Bitmap.h>

TileGrid::TileGrid(size_t softLimit, size_t hardLimit)
    :
    fGridLock("TileGridLock"),
    fCurrentMemoryUsage(0),
    fSoftMemoryLimit(softLimit),
    fHardMemoryLimit(hardLimit)
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
    auto it = fGrid.find(index);
    if (it != fGrid.end()) {
        if (it->second->GetBitmap())
            fCurrentMemoryUsage -= it->second->GetBitmap()->Size();
        else if (!it->second->GetCompressedData().empty())
            fCurrentMemoryUsage -= it->second->GetCompressedData().size();
        fGrid.erase(it);
        fRenderedLruMap.erase(index);
        fCompressedLruMap.erase(index);
    }
    fSieveCandidates.erase(index);
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
                if (tile->IsPinned()) {
                    fRenderedLruQueue.push_front(toEvict);
                    fRenderedLruMap[toEvict] = fRenderedLruQueue.begin();
                    continue;
                }
                if (tile->GetState() == RENDERED && tile->GetBitmap()) {
                    fCurrentMemoryUsage -= tile->GetBitmap()->Size();
                    BitmapPool::GetInstance().Release(tile->TakeBitmap());
                    tile->SetState(NEEDS_RENDER);
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
