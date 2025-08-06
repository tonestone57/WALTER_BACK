/*
 * Copyright 2025, Your Name <your.email@example.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#include "rendering/BitmapPool.h"
#include <Bitmap.h>
#include <Autolock.h>

BitmapPool&
BitmapPool::GetInstance()
{
    static BitmapPool instance;
    return instance;
}

BitmapPool::BitmapPool()
    :
    fLock("BitmapPoolLock")
{
}

BitmapPool::~BitmapPool()
{
    // unique_ptrs in the vector will handle deletion.
}

std::unique_ptr<BBitmap>
BitmapPool::Acquire(int width, int height)
{
    BAutolock locker(fLock);

    for (size_t i = 0; i < fPool.size(); ++i) {
        BBitmap* bitmap = fPool[i].get();
        if (bitmap->Bounds().Width() + 1 == width
            && bitmap->Bounds().Height() + 1 == height) {
            std::unique_ptr<BBitmap> foundBitmap = std::move(fPool[i]);
            fPool.erase(fPool.begin() + i);
            return foundBitmap;
        }
    }

    // No suitable bitmap in the pool, create a new one.
    BRect bounds(0, 0, width - 1, height - 1);
    return std::make_unique<BBitmap>(bounds, B_RGB32, true);
}

void
BitmapPool::Release(std::unique_ptr<BBitmap> bitmap)
{
    if (!bitmap)
        return;

    BAutolock locker(fLock);
    fPool.push_back(std::move(bitmap));
}
