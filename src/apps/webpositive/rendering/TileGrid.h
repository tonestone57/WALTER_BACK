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
#include <memory>

class TileGrid {
public:
    TileGrid();
    ~TileGrid();

    // Thread-safe method to get a tile. Returns nullptr if the tile
    // does not exist.
    Tile* GetTile(const TileIndex& index);

    // Thread-safe method to create a tile if it doesn't exist,
    // or get it if it does.
    Tile* GetOrCreateTile(const TileIndex& index);

    // Thread-safe method to remove a tile.
    void RemoveTile(const TileIndex& index);

    // Provides direct, locked access to the grid for more complex
    // operations, like iteration.
    BLocker* Locker() { return &fGridLock; }
    const std::unordered_map<TileIndex, std::unique_ptr<Tile>>& Map() const { return fGrid; }

private:
    BLocker fGridLock;
    std::unordered_map<TileIndex, std::unique_ptr<Tile>> fGrid;
};

#endif // TILE_GRID_H
