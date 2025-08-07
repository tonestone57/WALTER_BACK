/*
 * Copyright 2025, Your Name <your.email@example.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "rendering/Tile.h"
#include "rendering/TileGrid.h"
#include "rendering/RenderingConstants.h"
#include <Bitmap.h>
#include <memory>
#include <zstd.h>
#include <new>
#include <OS.h>

Tile::Tile(int32 x, int32 y)
    :
    fLock("TileLock"),
    fState(NEEDS_RENDER),
    fCompressionLevel(UNCOMPRESSED),
    fX(x),
    fY(y),
    fPinned(false),
    fAccessCount(0),
    fRenderComplexity(1.0f),
    fFrameImportance(1),
    fLastAccessTime(system_time()),
    fLastEvictionTime(0),
    fBitmap(nullptr),
    fBackBitmap(nullptr)
{
}

Tile::~Tile()
{
}

void
Tile::Reset(int32 x, int32 y)
{
    // Reset all members to their initial state, similar to the constructor.
    fState = NEEDS_RENDER;
    fCompressionLevel = UNCOMPRESSED;
    fX = x;
    fY = y;
    fPinned = false;
    fAccessCount = 0;
    fRenderComplexity = 1.0f;
    fFrameImportance = 1;
    fLastAccessTime = system_time();
    fLastEvictionTime = 0;

    // Clear any existing bitmap data
    fBitmap.reset();
    fBackBitmap.reset();
    fCompressedData.clear();
    fCompressedData.shrink_to_fit();
    fDirtyRegion.MakeEmpty();
}

void
Tile::SetBitmap(std::unique_ptr<BBitmap> bitmap)
{
    fBitmap = std::move(bitmap);
}

std::unique_ptr<BBitmap>
Tile::TakeBitmap()
{
    return std::move(fBitmap);
}

void
Tile::SetCompressedData(std::vector<uint8_t>&& data)
{
    fCompressedData = std::move(data);
}

bool
Tile::Compress(TileGrid* grid, CompressionLevel level)
{
    if (!fBitmap)
        return false;

    const uint8* uncompressedData = (const uint8*)fBitmap->Bits();
    size_t uncompressedSize = fBitmap->BitsLength();
    size_t compressedBound = ZSTD_compressBound(uncompressedSize);

    std::vector<uint8_t> compressedData(compressedBound);

    int zstdLevel = -5;
    if (level == COMPRESSED_HIGH)
        zstdLevel = 2;

    size_t compressedSize = ZSTD_compress(compressedData.data(), compressedBound,
        uncompressedData, uncompressedSize, zstdLevel);

    if (ZSTD_isError(compressedSize)) {
        return false;
    }

    compressedData.resize(compressedSize);

    grid->UpdateMemoryUsage(compressedSize - uncompressedSize);

    SetCompressedData(std::move(compressedData));
    fBitmap.reset();
    fCompressionLevel = level;
    SetState(COMPRESSED);
    return true;
}

bool
Tile::Recompress(TileGrid* grid, CompressionLevel newLevel)
{
    if (fCompressedData.empty() || fCompressionLevel == newLevel)
        return false;

    size_t oldCompressedSize = fCompressedData.size();

    // Decompress to a temporary buffer
    size_t decompressedSize = ZSTD_getFrameContentSize(fCompressedData.data(), oldCompressedSize);
    if (decompressedSize == ZSTD_CONTENTSIZE_ERROR || decompressedSize == ZSTD_CONTENTSIZE_UNKNOWN)
        return false;

    std::vector<uint8_t> decompressedData(decompressedSize);
    size_t result = ZSTD_decompress(decompressedData.data(), decompressedSize, fCompressedData.data(), oldCompressedSize);
    if (ZSTD_isError(result))
        return false;

    // Re-compress from the temporary buffer
    int zstdLevel = -5;
    if (newLevel == COMPRESSED_HIGH)
        zstdLevel = 2;

    size_t newCompressedBound = ZSTD_compressBound(decompressedSize);
    std::vector<uint8_t> newCompressedData(newCompressedBound);
    size_t newCompressedSize = ZSTD_compress(newCompressedData.data(), newCompressedBound,
        decompressedData.data(), decompressedSize, zstdLevel);

    if (ZSTD_isError(newCompressedSize)) {
        return false;
    }

    newCompressedData.resize(newCompressedSize);

    grid->UpdateMemoryUsage(newCompressedSize - oldCompressedSize);

    SetCompressedData(std::move(newCompressedData));
    fCompressionLevel = newLevel;
    SetState(COMPRESSED);
    return true;
}

bool
Tile::Decompress(TileGrid* grid)
{
    if (fCompressedData.empty())
        return false;

    size_t compressedSize = fCompressedData.size();
    size_t decompressedSize = ZSTD_getFrameContentSize(fCompressedData.data(), compressedSize);
    if (decompressedSize == ZSTD_CONTENTSIZE_ERROR || decompressedSize == ZSTD_CONTENTSIZE_UNKNOWN)
        return false;

    std::unique_ptr<BBitmap> bitmap = std::make_unique<BBitmap>(kTileRect, B_RGB32, true);
    if (!bitmap || bitmap->BitsLength() != decompressedSize)
        return false;

    size_t result = ZSTD_decompress(bitmap->Bits(), decompressedSize,
        fCompressedData.data(), compressedSize);

    if (ZSTD_isError(result))
        return false;

    fCompressedData.clear();
    fCompressedData.shrink_to_fit();

    grid->UpdateMemoryUsage(decompressedSize - compressedSize);

    SetBitmap(std::move(bitmap));
    SetState(RENDERED);
    return true;
}
