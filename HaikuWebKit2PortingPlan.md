# Haiku WebKit2 Porting Plan

This document outlines the high-level task list for porting the Haiku WebKit port to the modern, multi-process WebKit2 architecture.

## Phase 1: Establish the Multi-Process Model

- Create the basic directory structure for the Haiku port within the `Source/WebKit` directory, mirroring other ports (`UIProcess`, `WebProcess`).
- Implement the main entry point for the `WebProcess`.
- Create a Haiku-specific `ProcessLauncher` for the `UIProcess` to handle launching and managing the `WebProcess`.

## Phase 2: Implement Core WebProcess Skeletons

- Create skeleton implementations for the core classes in the `WebProcess`:
    - `WebPageHaiku`: Manages the web page in the `WebProcess`.
    - `WebFrameHaiku`: Represents a frame within the web page.
    - `WebChromeClientHaiku`: Handles UI-related tasks for the `WebProcess` (e.g., alerts, window creation).
    - `WebFrameLoaderClientHaiku`: Manages frame loading and navigation.
    - `DrawingAreaHaiku`: Manages the drawing and compositing of the web page.

## Phase 3: Implement Core UIProcess Skeletons

- Create skeleton implementations for the core classes in the `UIProcess`:
    - `WebPageProxyHaiku`: The `UIProcess` representation of a web page.
    - `WebFrameProxyHaiku`: The `UIProcess` representation of a frame.
    - `WebProcessProxyHaiku`: Represents a `WebProcess` instance.
    - `DrawingAreaProxyHaiku`: The `UIProcess` side of the drawing area, responsible for displaying the web content.

## Phase 4: Establish IPC Communication

- Define and implement the IPC messages that will be exchanged between the `UIProcess` and `WebProcess` for the Haiku port.
- Implement the necessary message handlers in the corresponding classes (`WebPage`/`WebPageProxy`, `WebFrame`/`WebFrameProxy`, etc.).
- This includes handling navigation, UI requests, and other interactions.

## Phase 5: Implement Drawing and Compositing

- Implement the `DrawingArea` and `DrawingAreaProxy` to handle the rendering and display of web content.
- This will involve integrating with Haiku's graphics and windowing systems.
- This is likely the most complex phase and will require significant platform-specific code.
