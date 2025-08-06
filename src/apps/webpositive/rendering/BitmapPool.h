/*
 * Copyright 2025, Your Name <your.email@example.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef BITMAP_POOL_H
#define BITMAP_POOL_H

#include <Locker.h>
#include <memory>
#include <vector>

class BBitmap;

class BitmapPool {
public:
    static BitmapPool& GetInstance();

    std::unique_ptr<BBitmap> Acquire(int width, int height);
    void Release(std::unique_ptr<BBitmap> bitmap);

private:
    BitmapPool();
    ~BitmapPool();
    BitmapPool(const BitmapPool&) = delete;
    BitmapPool& operator=(const BitmapPool&) = delete;

    BLocker fLock;
    std::vector<std::unique_ptr<BBitmap>> fPool;
};

#endif // BITMAP_POOL_H
