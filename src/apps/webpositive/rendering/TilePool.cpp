/*
 * Copyright 2025, Your Name <your.email@example.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "rendering/TilePool.h"

TilePool&
TilePool::GetInstance()
{
    static TilePool instance;
    return instance;
}

std::unique_ptr<Tile>
TilePool::Acquire(int32 x, int32 y)
{
    std::lock_guard<std::mutex> lock(fMutex);

    if (!fPool.empty()) {
        std::unique_ptr<Tile> tile = std::move(fPool.back());
        fPool.pop_back();
        // The Reset method will be added in the next step.
        // For now, this will cause a compile error, which is expected.
        tile->Reset(x, y);
        return tile;
    }

    return std::make_unique<Tile>(x, y);
}

void
TilePool::Release(std::unique_ptr<Tile> tile)
{
    std::lock_guard<std::mutex> lock(fMutex);
    fPool.push_back(std::move(tile));
}
