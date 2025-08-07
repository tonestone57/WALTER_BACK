/*
 * Copyright 2025, The Haiku Project.
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef RENDERING_CONSTANTS_H
#define RENDERING_CONSTANTS_H

#include <GraphicsDefs.h>
#include <Rect.h>

// Using a constant for the tile size is a good practice. It makes the code
// more maintainable and is a prerequisite for future features like
// adaptive tile sizing.
const int32 kTileSize = 256;

// A pre-calculated BRect for the tile dimensions.
const BRect kTileRect(0, 0, kTileSize - 1, kTileSize - 1);

#endif // RENDERING_CONSTANTS_H
