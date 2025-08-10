set(WebKitLegacy_PRIVATE_INCLUDE_DIRECTORIES
    "${WEBKITLEGACY_DIR}"
    "${WEBKITLEGACY_DIR}/haiku"
    "${WEBKITLEGACY_DIR}/haiku/API"
    "${WEBKITLEGACY_DIR}/mac"
    "${WEBKITLEGACY_DIR}/mac/History"
    "${WEBKITLEGACY_DIR}/mac/Misc"
    "${WEBKITLEGACY_DIR}/mac/Plugins"
    "${WEBKITLEGACY_DIR}/mac/Storage"
    "${WEBKITLEGACY_DIR}/mac/WebCoreSupport"
    "${WEBKITLEGACY_DIR}/mac/WebView"
    "${WEBKITLEGACY_DIR}/Storage"
    "${WEBKITLEGACY_DIR}/WebCoreSupport"
)

if (HAVE_HAIKU_BUILD_FEATURE)
    list(APPEND WebKitLegacy_UNIFIED_SOURCE_LIST_FILES
        "${WEBKITLEGACY_DIR}/SourcesHaiku.txt"
    )
endif ()

set(WebKitLegacy_LIBRARIES
    WebCore
)
