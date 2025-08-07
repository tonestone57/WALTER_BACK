/*
 * Copyright 2025, Your Name <your.email@example.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef TILE_POOL_H
#define TILE_POOL_H

#include "rendering/Tile.h"
#include <memory>
#include <vector>
#include <mutex>

class TilePool {
public:
    static TilePool& GetInstance();

    std::unique_ptr<Tile> Acquire(int32 x, int32 y);
    void Release(std::unique_ptr<Tile> tile);

    // Delete copy and move constructors and assign operators
    TilePool(const TilePool&) = delete;
    TilePool& operator=(const TilePool&) = delete;
    TilePool(TilePool&&) = delete;
    TilePool& operator=(TilePool&&) = delete;

private:
    TilePool() = default;
    ~TilePool() = default;

    std::mutex fMutex;
    std::vector<std::unique_ptr<Tile>> fPool;
};

#endif // TILE_POOL_H
