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
#include <Region.h>

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

enum CompressionLevel {
    UNCOMPRESSED,
    COMPRESSED_FAST,
    COMPRESSED_HIGH
};

class Tile {
    friend class BWebPage;
    friend class TileGrid;
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

    bool Compress(TileGrid* grid, CompressionLevel level);
    bool Recompress(TileGrid* grid, CompressionLevel newLevel);
    bool Decompress(TileGrid* grid);

    int32 GetX() const { return fX; }
    int32 GetY() const { return fY; }

    void Pin() { fPinned = true; }
    void Unpin() { fPinned = false; }
    bool IsPinned() const { return fPinned; }

    uint32 AccessCount() const { return fAccessCount; }
    void IncrementAccessCount() { fAccessCount++; }
    void DecayAccessCount() { fAccessCount /= 2; }

    void AddDirtyRect(const BRect& rect) { fDirtyRegion.Include(rect); }
    void ClearDirtyRegion() { fDirtyRegion.MakeEmpty(); }
    const BRegion& DirtyRegion() const { return fDirtyRegion; }

    bigtime_t LastAccessTime() const { return fLastAccessTime; }
    void SetLastAccessTime(bigtime_t time) { fLastAccessTime = time; }

    bigtime_t LastEvictionTime() const { return fLastEvictionTime; }
    void SetLastEvictionTime(bigtime_t time) { fLastEvictionTime = time; }

    CompressionLevel GetCompressionLevel() const { return fCompressionLevel; }

    float GetRenderComplexity() const { return fRenderComplexity; }
    void SetRenderComplexity(float complexity) { fRenderComplexity = complexity; }

private:
    BLocker fLock;
    TileState fState;
    CompressionLevel fCompressionLevel;
    int32 fX;
    int32 fY;
    bool fPinned;
    uint32_t fAccessCount;
    float fRenderComplexity;
    bigtime_t fLastAccessTime;
    bigtime_t fLastEvictionTime;

    std::unique_ptr<BBitmap> fBitmap;
    std::unique_ptr<BBitmap> fBackBitmap;
    std::vector<uint8_t> fCompressedData;
    BRegion fDirtyRegion;
};

#endif // TILE_H
