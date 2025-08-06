# Proposal: Tile-Based Rendering for WebPositive

## 1. Executive Summary

To significantly improve the performance, responsiveness, and memory efficiency of WebPositive, this document proposes a transition from the current direct rendering model to a modern, **multi-threaded, tile-based rendering system**. This change would be implemented within the Haiku WebKit port.

The core idea is to break the webpage view into a grid of smaller tiles. These tiles can then be rendered independently and in parallel by a pool of worker threads. To manage memory, rendered tiles that are off-screen will be compressed using a fast, modern algorithm. A sophisticated **SIEVE1+LRU hybrid caching system** will manage the lifecycle of these tiles, ensuring that only high-value, reusable content is cached, thus maximizing performance and memory efficiency.

This architecture is highly extensible and allows for numerous advanced optimizations, which are also detailed in this proposal.

---

## 2. Core Proposal: System Design

This section details the fundamental components of the proposed rendering system.

### 2.1. Tile-Based Rendering

The foundation of the new system is a tile-based approach.

*   **Grid Size:** A fixed tile size of **256x256 pixels**. This is a well-tested industry standard that provides a good balance between rendering granularity and management overhead.
*   **Tile Grid Management:** A **sparse two-dimensional grid**, likely implemented as a hash map (`std::unordered_map`), will be used to store and manage the tiles. This is memory-efficient as it only allocates memory for tiles that are actually needed.
*   **Rendering Strategy:** The rendering process will be modified to draw content onto individual tile bitmaps. This is achieved by translating the graphics context's coordinate system for each tile before invoking WebCore's painting routines.
*   **Invalidation:** The system will hook into WebKit's existing invalidation mechanism. When a part of the page becomes "dirty," only the tiles that intersect with that region will be marked for re-rendering.

### 2.2. Multi-Threading

To accelerate rendering, a thread pool will be used to render multiple tiles concurrently.

*   **Thread Pool:** A pool of worker threads will be created, with the number of threads based on the available CPU cores (e.g., `number_of_cores - 1`).
*   **Work Queue:** A thread-safe work queue will be used to distribute render tasks to the worker threads.
*   **Synchronization:** Access to shared data structures, particularly the tile grid, will be protected by **mutexes** to prevent race conditions and ensure data integrity.

### 2.3. Compression

To manage the memory footprint of the rendered tiles, especially on large pages, off-screen tiles will be compressed.

*   **Algorithm:** **Zstandard (zstd)** is recommended. Its exceptional decompression speed is critical for ensuring that scrolling remains smooth when compressed tiles need to be displayed. It offers a better performance profile for this use case than older algorithms like zlib.
*   **Workflow:** Once a tile is rendered and is not visible in the viewport, a worker thread can compress its bitmap data. The original bitmap is then deleted, and the compressed data is stored in the tile object.

### 2.4. Caching (SIEVE1+LRU Hybrid Model)

A sophisticated hybrid caching system is essential for intelligently managing the lifecycle of tiles and enforcing a memory budget. This approach avoids cache pollution from one-time-use tiles and improves the hit rate.

*   **Algorithm:** A **SIEVE1+LRU** policy.
    *   **SIEVE1 Filtering (Admission Control):** A tile is only admitted into the main cache if it has been accessed more than once. This "sieves" out tiles that are only seen briefly (e.g., during a fast scroll) and are unlikely to be needed again, preventing them from displacing more valuable tiles.
    *   **LRU Eviction:** Once a tile passes the sieve and is admitted to the cache, it is managed by a standard **Least Recently Used (LRU)** policy. When the cache is full, the least recently used tile is evicted.
*   **Cache Size:** A **dynamic cache size based on a memory limit** (e.g., 128 MB, user-configurable) is recommended.
*   **Workflow:**
    1.  When a tile is accessed for the first time, its access count is incremented, and it's added to a temporary "sieve candidates" list. It is **not** cached yet.
    2.  If the tile is accessed again, it passes the sieve and is admitted to the main LRU cache.
    3.  If the cache's memory limit is exceeded, tiles are evicted from the back of the LRU list.

---

## 3. Advanced Optimizations and Enhancements

This section details the many excellent ideas proposed for further enhancing the core system.

### 3.1. General Enhancements

*   **Soft and Hard Cache Limits:** Introduce two levels of memory limits. A "soft" limit triggers a gentle, low-priority eviction process, while a "hard" limit forces an immediate and aggressive cleanup to prevent crashes.
*   **Eviction Hints from WebCore:** Leverage WebKit's internal knowledge of the page structure to make more intelligent eviction decisions.
*   **High-DPI Display Support:** Ensure the entire tiling system is DPI-aware, defining tile sizes in logical units and scaling them correctly for high-resolution displays.
*   **Memory Pressure Awareness:** Allow the browser to respond to system-level memory pressure notifications by dynamically adjusting its cache limits.
*   **Fallback for Low-Memory Systems:** For devices with very limited RAM, provide a simplified rendering path (e.g., smaller cache, no compression) to ensure usability.

### 3.2. Tile System Optimizations

*   **Adaptive Tile Size:** Dynamically adjust the tile size based on the zoom level or DPI to optimize for different viewing contexts.
*   **Partial Tile Invalidation:** To avoid re-rendering an entire tile for a small change (like a blinking cursor), track "dirty rects" *within* each tile.
*   **Asynchronous Decompression:** Decompress tiles on background threads just before they enter the viewport to avoid blocking the UI thread.
*   **Double Buffering for Tiles:** Use double buffering when updating a tile's content to prevent visual artifacts like flickering.
*   **Predictive Tile Prefetching:** Analyze scroll velocity and direction to anticipate which tiles will be needed soon and begin rendering or decompressing them in advance.

### 3.3. Multi-Threading Optimizations

*   **Task Coalescing:** Merge multiple render requests for the same tile into a single task to avoid redundant work.
*   **Render Budgeting:** To maintain a smooth UI, limit the number of tiles rendered per frame, ensuring the main thread is always available to handle user input.
*   **Thread Affinity Hints:** On some systems, pinning rendering threads to specific CPU cores can reduce cache thrashing and improve performance.

### 3.4. Compression and Caching Optimizations

*   **SIEVE1 Cache Filtering:** Use a SIEVE1-based filter as an admission controller to the main LRU cache, preventing cache pollution from one-time-use tiles.
*   **Decaying Access Count:** Gradually decay the access count of tiles over time to avoid stale tiles remaining in the cache indefinitely.
*   **Pinning Visible Tiles:** Explicitly "pin" tiles that are currently in the viewport to prevent them from being evicted.
*   **Compression Tiering:** Apply stronger, slower compression to tiles that haven't been accessed in a long time, and lighter, faster compression to more recent tiles.
*   **Compression-aware Eviction:** Make the eviction policy smarter by considering not just when a tile was last used, but also how expensive it would be to re-render or decompress it.
*   **Segmented LRU Cache:** Maintain separate LRU queues for compressed and uncompressed tiles to be more strategic about eviction.
*   **Eviction Grace Period:** Delay the eviction of recently used tiles to avoid "thrashing" during rapid scrolling.
*   **Cache Warm-Up:** Proactively render and cache tiles in and around the initial viewport to improve perceived page load time.

### 3.5. Code Quality and Maintenance

*   **Tile Coordinate Abstraction:** Use a dedicated `TileIndex` class or struct with proper hashing and comparison logic to simplify grid management and improve code clarity.
*   **Resource Pooling:** Implement object pools for `BBitmap` objects (`Tile Pooling` and `Bitmap Rehydration Pool`) to reduce the overhead of memory allocation and deallocation.

---

## 4. Implementation Task List

This is a high-level, phased task list for implementing the proposed system.

### **Phase 1: Core Tiling Infrastructure (Single-Threaded)**
1.  **[Setup]** Set up a build environment for the Haiku WebKit port.
2.  **[Data Structures]** Implement the `Tile`, `TileIndex`, and `TileGrid` classes.
3.  **[Rendering Interception]** Modify the WebKit port's painting code to iterate through visible tiles.
4.  **[Tiled Drawing]** Implement the logic to draw content onto individual tile bitmaps.
5.  **[Invalidation]** Hook into WebKit's invalidation system to mark tiles as dirty.

### **Phase 2: Multi-Threading and Compression**
1.  **[Multi-Threading]** Implement the thread pool, work queue, and synchronization primitives.
2.  **[Compression]** Integrate the `zstd` library and implement the tile compression/decompression workflow.

### **Phase 3: Caching and Memory Management**
1.  **[Cache Logic]** Implement the **SIEVE1+LRU** cache mechanism.
2.  **[Memory Limits]** Implement cache size tracking and the eviction logic.
3.  **[Resource Pooling]** Implement a `BBitmap` pool.

### **Phase 4: Advanced Optimizations**
1.  **[Predictive Prefetching]** Implement scroll-aware pre-rendering.
2.  **[Advanced Caching]** Implement features like soft/hard limits, eviction hints, and segmented queues.
3.  **[Advanced Rendering]** Implement features like partial invalidation and High-DPI support.
4.  **[Advanced Threading]** Implement features like task coalescing and render budgeting.

### **Phase 5: Testing and Deployment**
1.  **[Testing]** Perform rigorous automated and manual testing, including thread sanitizer analysis.
2.  **[Performance Tuning]** Profile and tune the system parameters for optimal performance.
3.  **[Configuration]** Expose key parameters as user-configurable settings.
4.  **[Deployment]** Integrate the updated WebKit port back into the main Haiku build.
