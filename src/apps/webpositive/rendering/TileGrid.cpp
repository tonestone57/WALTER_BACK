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
        fCurrentMemoryUsage -= it->second->GetBitmap()->Size();
        fGrid.erase(it);
        fLruMap.erase(index);
        // The iterator in fLruQueue is now invalid, but it's okay
        // because we've removed the corresponding entry from fLruMap.
    }
    fSieveCandidates.erase(index);
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

    while (fCurrentMemoryUsage > limit && !fLruQueue.empty()) {
        TileIndex toEvict = fLruQueue.back();
        fLruQueue.pop_back();

        auto it = fGrid.find(toEvict);
        if (it != fGrid.end()) {
            // Release the bitmap back to the pool.
            BitmapPool::GetInstance().Release(
                std::unique_ptr<BBitmap>(it->second->GetBitmap()));
            fCurrentMemoryUsage -= it->second->GetBitmap()->Size();
            it->second->SetState(TileState::NEEDS_RENDER);
        }
        fLruMap.erase(toEvict);
    }
}

void
TileGrid::_PromoteTile(const TileIndex& index)
{
    // If the tile is in the LRU queue, move it to the front.
    auto it = fLruMap.find(index);
    if (it != fLruMap.end()) {
        fLruQueue.erase(it->second);
        fLruQueue.push_front(index);
        fLruMap[index] = fLruQueue.begin();
    } else {
        // SIEVE1: If it's a candidate, promote it to the main cache.
        if (fSieveCandidates.count(index)) {
            fSieveCandidates.erase(index);
            fLruQueue.push_front(index);
            fLruMap[index] = fLruQueue.begin();
        }
    }
}
