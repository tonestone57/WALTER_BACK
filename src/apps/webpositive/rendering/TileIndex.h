/*
 * Copyright 2025, Your Name <your.email@example.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef TILE_INDEX_H
#define TILE_INDEX_H

#include <SupportDefs.h>
#include <functional>

struct TileIndex {
    int32 row;
    int32 col;

    bool operator==(const TileIndex& other) const
    {
        return row == other.row && col == other.col;
    }
};

// Provide a hash function for TileIndex so it can be used as a key
// in std::unordered_map.
namespace std {
    template <>
    struct hash<TileIndex>
    {
        std::size_t operator()(const TileIndex& index) const
        {
            // A simple hash combination function.
            // This is a common pattern for hashing pairs of integers.
            std::size_t h1 = std::hash<int32>()(index.row);
            std::size_t h2 = std::hash<int32>()(index.col);
            return h1 ^ (h2 << 1);
        }
    };
}

#endif // TILE_INDEX_H
