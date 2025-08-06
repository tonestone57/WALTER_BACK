/*
 * Copyright 2025, Your Name <your.email@example.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef TILE_H
#define TILE_H

#include <Locker.h>
#include <memory>

// Forward declaration to avoid including the full BBitmap header here.
class BBitmap;

enum TileState {
    // The tile has not been rendered yet.
    NEEDS_RENDER,
    // The tile is currently being rendered by a worker thread.
    RENDERING,
    // The tile has been rendered and its BBitmap is available.
    RENDERED,
    // The tile's bitmap has been compressed to save memory.
    COMPRESSED,
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

    // In a real implementation, this would hold compressed data.
    // For now, it's a placeholder.
    char* GetCompressedData() const { return fCompressedData; }
    void SetCompressedData(char* data, size_t size);
    size_t GetCompressedSize() const { return fCompressedSize; }


private:
    BLocker fLock;
    TileState fState;
    int32 fX;
    int32 fY;

    std::unique_ptr<BBitmap> fBitmap;
    char* fCompressedData;
    size_t fCompressedSize;
};

#endif // TILE_H
