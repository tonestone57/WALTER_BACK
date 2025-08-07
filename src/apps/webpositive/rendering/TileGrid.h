/*
 * Copyright 2025, Your Name <your.email@example.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef TILE_GRID_H
#define TILE_GRID_H

#include "rendering/Tile.h"
#include "rendering/TileIndex.h"

#include <Locker.h>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <memory>

class BWebPage;
class ThreadPool;

class TileGrid {
public:
    TileGrid(BWebPage* webPage, ThreadPool* threadPool, int32 tileSize, size_t softLimit, size_t hardLimit);
    ~TileGrid();

    void PrefetchTiles(BRect viewport, BPoint scrollVelocity);
    void MarkTileAsDirty(const TileIndex& index);
    void ProcessDirtyTiles(int32& renderBudget);
    Tile* GetTile(const TileIndex& index);
    Tile* GetOrCreateTile(const TileIndex& index);
    void RemoveTile(const TileIndex& index);

    void SetMemoryLimits(size_t softLimit, size_t hardLimit);
    BRect Scroll(int xOffset, int yOffset, const BRect& rectToScroll);
    void EvictTiles(bool aggressive);
    void UpdateMemoryUsage(int64_t delta);

    void UpgradeCompressionTier();

    void MoveToCompressedQueue(const TileIndex& index);
    void MoveToRenderedQueue(const TileIndex& index);

    BLocker* Locker() { return &fGridLock; }
    const std::unordered_map<TileIndex, std::unique_ptr<Tile>>& Map() const { return fGrid; }
    void SetMap(std::unordered_map<TileIndex, std::unique_ptr<Tile>>&& map);

private:
    void _PromoteTile(const TileIndex& index);

    BLocker fGridLock;
    std::unordered_map<TileIndex, std::unique_ptr<Tile>> fGrid;

    // Caching
    std::unordered_set<TileIndex> fSieveCandidates;
    std::list<TileIndex> fRenderedLruQueue;
    std::unordered_map<TileIndex, std::list<TileIndex>::iterator> fRenderedLruMap;
    std::list<TileIndex> fCompressedLruQueue;
    std::unordered_map<TileIndex, std::list<TileIndex>::iterator> fCompressedLruMap;

    size_t fCurrentMemoryUsage;
    size_t fSoftMemoryLimit;
    size_t fHardMemoryLimit;

    BWebPage* fWebPage;
    ThreadPool* fThreadPool;
    int32 fTileSize;

    BLocker fDirtyTilesLock;
    std::unordered_set<TileIndex> fDirtyTiles;
};

#endif // TILE_GRID_H
