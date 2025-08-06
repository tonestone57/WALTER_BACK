/*
 * Copyright 2025, Your Name <your.email@example.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "rendering/Tile.h"
#include <Bitmap.h>
#include <memory>

Tile::Tile(int32 x, int32 y)
    :
    fLock("TileLock"),
    fState(NEEDS_RENDER),
    fX(x),
    fY(y),
    fBitmap(nullptr),
    fCompressedData(nullptr),
    fCompressedSize(0)
{
}

Tile::~Tile()
{
    // fBitmap is managed by unique_ptr and will be deleted automatically.
    // fCompressedData must be manually deleted.
    delete[] fCompressedData;
}

void
Tile::SetBitmap(std::unique_ptr<BBitmap> bitmap)
{
    fBitmap = std::move(bitmap);
}

void
Tile::SetCompressedData(char* data, size_t size)
{
    // If we already have compressed data, delete it first.
    delete[] fCompressedData;

    fCompressedData = data;
    fCompressedSize = size;
}
