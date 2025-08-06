/*
 * Copyright 2025, Your Name <your.email@example.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef TILE_H
#define TILE_H

#include <Locker.h>
#include <memory>
#include <vector>
#include <cstdint>

// Forward declaration to avoid including the full BBitmap header here.
class BBitmap;
class TileGrid;

enum TileState {
    // The tile has not been rendered yet.
    NEEDS_RENDER,
    // The tile is currently being rendered by a worker thread.
    RENDERING,
    // The tile has been rendered and its BBitmap is available.
    RENDERED,
    // The tile is being compressed.
    COMPRESSING,
    // The tile's bitmap has been compressed to save memory.
    COMPRESSED,
    // The tile is being decompressed.
    DECOMPRESSING,
    // The tile is queued for eviction.
    EVICTING
};

class Tile {
public:
    Tile(int32 x, int32 y);
    ~Tile();

    bool Lock() { return fLock.Lock(); }
    void Unlock() { fLock.Unlock(); }

    TileState GetState() const { return fState; }
    void SetState(TileState state) { fState = state; }

    BBitmap* GetBitmap() const { return fBitmap.get(); }
    void SetBitmap(std::unique_ptr<BBitmap> bitmap);
    std::unique_ptr<BBitmap> TakeBitmap();

    const std::vector<uint8_t>& GetCompressedData() const { return fCompressedData; }
    void SetCompressedData(std::vector<uint8_t>&& data);

    bool Compress(TileGrid* grid);
    bool Decompress(TileGrid* grid);

    int32 GetX() const { return fX; }
    int32 GetY() const { return fY; }

    void Pin() { fPinned = true; }
    void Unpin() { fPinned = false; }
    bool IsPinned() const { return fPinned; }

private:
    BLocker fLock;
    TileState fState;
    int32 fX;
    int32 fY;
    bool fPinned;

    std::unique_ptr<BBitmap> fBitmap;
    std::vector<uint8_t> fCompressedData;
};

#endif // TILE_H
