/*
 * Copyright 2025, Your Name <your.email@example.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "rendering/TileGrid.h"
#include "rendering/Tile.h"

#include <Autolock.h>

TileGrid::TileGrid()
    :
    fGridLock("TileGridLock")
{
}

TileGrid::~TileGrid()
{
    // All tiles are owned by unique_ptr in the map and will be
    // deleted automatically when the map is destroyed.
}

Tile*
TileGrid::GetTile(const TileIndex& index)
{
    BAutolock locker(fGridLock);
    auto it = fGrid.find(index);
    if (it != fGrid.end())
        return it->second.get();
    return nullptr;
}

Tile*
TileGrid::GetOrCreateTile(const TileIndex& index)
{
    BAutolock locker(fGridLock);
    auto it = fGrid.find(index);
    if (it != fGrid.end())
        return it->second.get();

    // Tile does not exist, create it.
    auto newTile = std::make_unique<Tile>(index.col, index.row);
    Tile* tilePtr = newTile.get();
    fGrid[index] = std::move(newTile);
    return tilePtr;
}

void
TileGrid::RemoveTile(const TileIndex& index)
{
    BAutolock locker(fGridLock);
    fGrid.erase(index);
}
