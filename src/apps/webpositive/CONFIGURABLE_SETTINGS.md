# Proposed User-Facing Configuration Settings

This document outlines a list of potential user-facing settings that could be exposed in a new "Performance" or "Advanced" section of the WebPositive settings panel. These settings would give users fine-grained control over the tile-based rendering engine.

---

### Caching

*   **Tile Cache Memory Limit**
    *   **Summary:** Allows the user to set the maximum amount of RAM (in megabytes) that the browser can use to cache rendered web page tiles. Increasing this can improve performance on systems with lots of RAM, while decreasing it can reduce the browser's memory footprint.
    *   **Type:** Slider or integer input.
    *   **Default:** 64MB or 128MB (based on system memory).

### Compression

*   **Enable Off-screen Tile Compression**
    *   **Summary:** A master switch to turn tile compression on or off. Disabling it might offer a slight performance boost on high-end systems at the cost of significantly higher memory usage.
    *   **Type:** Checkbox.
    *   **Default:** Enabled.

*   **Compression Levels (Advanced)**
    *   **Summary:** Two separate settings to control the "fast" and "high" Zstandard compression levels. This would allow advanced users to fine-tune the trade-off between CPU usage during compression and the amount of memory saved.
    *   **Type:** Two integer inputs.
    *   **Default:** Fast: -5, High: 2.

### Multi-Threading

*   **Number of Rendering Threads**
    *   **Summary:** Allows the user to manually override the number of background threads used for rendering tiles. This could be useful for limiting CPU usage on battery-powered devices or for performance tuning on specific hardware.
    *   **Type:** Integer input or dropdown.
    *   **Default:** `number_of_cores - 1`.

### Feature Toggles (Advanced)

*   **Enable Predictive Prefetching**
    *   **Summary:** A checkbox to enable or disable the feature that pre-renders and decompresses tiles in the direction of scrolling. Disabling it would reduce background activity at the cost of potentially less smooth scrolling.
    *   **Type:** Checkbox.
    *   **Default:** Enabled.
