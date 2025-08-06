/*
 * Copyright 2025, Your Name <your.email@example.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "rendering/Tile.h"
#include <Bitmap.h>
#include <memory>
#include <zstd.h>
#include <new>

Tile::Tile(int32 x, int32 y)
    :
    fLock("TileLock"),
    fState(NEEDS_RENDER),
    fX(x),
    fY(y),
    fBitmap(nullptr)
{
}

Tile::~Tile()
{
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
Tile::Compress()
{
    if (!fBitmap)
        return false;

    const uint8* uncompressedData = (const uint8*)fBitmap->Bits();
    size_t uncompressedSize = fBitmap->BitsLength();
    size_t compressedBound = ZSTD_compressBound(uncompressedSize);

    std::vector<uint8_t> compressedData(compressedBound);

    size_t compressedSize = ZSTD_compress(compressedData.data(), compressedBound,
        uncompressedData, uncompressedSize, 1);

    if (ZSTD_isError(compressedSize)) {
        return false;
    }

    compressedData.resize(compressedSize);
    SetCompressedData(std::move(compressedData));
    fBitmap.reset();
    SetState(COMPRESSED);
    return true;
}

bool
Tile::Decompress()
{
    if (fCompressedData.empty())
        return false;

    size_t decompressedSize = ZSTD_getFrameContentSize(fCompressedData.data(), fCompressedData.size());
    if (decompressedSize == ZSTD_CONTENTSIZE_ERROR || decompressedSize == ZSTD_CONTENTSIZE_UNKNOWN)
        return false;

    std::unique_ptr<BBitmap> bitmap = std::make_unique<BBitmap>(BRect(0, 0, 255, 255), B_RGB32, true);
    if (!bitmap || bitmap->BitsLength() != decompressedSize)
        return false;

    size_t result = ZSTD_decompress(bitmap->Bits(), decompressedSize,
        fCompressedData.data(), fCompressedData.size());

    if (ZSTD_isError(result))
        return false;

    fCompressedData.clear();
    fCompressedData.shrink_to_fit();

    SetBitmap(std::move(bitmap));
    SetState(RENDERED);
    return true;
}
