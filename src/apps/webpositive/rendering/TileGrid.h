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

class TileGrid {
public:
    TileGrid(size_t softLimit, size_t hardLimit);
    ~TileGrid();

    Tile* GetTile(const TileIndex& index);
    Tile* GetOrCreateTile(const TileIndex& index);
    void RemoveTile(const TileIndex& index);

    void SetMemoryLimits(size_t softLimit, size_t hardLimit);
    void EvictTiles(bool aggressive);
    void UpdateMemoryUsage(int64_t delta);

    BLocker* Locker() { return &fGridLock; }
    const std::unordered_map<TileIndex, std::unique_ptr<Tile>>& Map() const { return fGrid; }
    void SetMap(std::unordered_map<TileIndex, std::unique_ptr<Tile>>&& map);

private:
    void _PromoteTile(const TileIndex& index);

    BLocker fGridLock;
    std::unordered_map<TileIndex, std::unique_ptr<Tile>> fGrid;

    // Caching
    std::unordered_set<TileIndex> fSieveCandidates;
    std::list<TileIndex> fLruQueue;
    std::unordered_map<TileIndex, std::list<TileIndex>::iterator> fLruMap;

    size_t fCurrentMemoryUsage;
    size_t fSoftMemoryLimit;
    size_t fHardMemoryLimit;
};

#endif // TILE_GRID_H
