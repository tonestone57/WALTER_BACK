LIST(APPEND WebKit_INCLUDE_DIRECTORIES
    "${CMAKE_SOURCE_DIR}/Source"
    "${DERIVED_SOURCES_JAVASCRIPTCORE_DIR}"
    "${WEBKIT_DIR}/haiku"
    "${WEBKIT_DIR}/haiku/API"
    "${WEBKIT_DIR}/haiku/WebCoreSupport"
    "${WTF_DIR}"
    "${LIBXML2_INCLUDE_DIR}"
    "${LIBXSLT_INCLUDE_DIR}"
    "${SQLITE_INCLUDE_DIR}"
    "${CMAKE_BINARY_DIR}"
    "${FORWARDING_HEADERS_DIR}"
    /system/develop/headers/private/netservices
)

# These folders have includes with the same name as Haiku system ones. So we
# add them with -iquote only, as a way to reach the Haiku includes with
# #include <>
SET(WebKit_LOCAL_INCLUDE_DIRECTORIES
    "${FORWARDING_HEADERS_DIR}/WebCore"
    "${WEBCORE_DIR}/Modules/notifications" # Notification.h
    "${WEBCORE_DIR}/platform/text" # DateTimeFormat.h
)

foreach(inc ${WebKit_LOCAL_INCLUDE_DIRECTORIES})
    ADD_DEFINITIONS(-iquote ${inc})
endforeach(inc)

IF (ENABLE_VIDEO_TRACK)
    LIST(APPEND WebKit_INCLUDE_DIRECTORIES
        "${WEBCORE_DIR}/html/track"
    )
ENDIF ()

add_definitions("-include WebKitPrefix.h")
add_definitions("-D_DEFAULT_SOURCE")

list(APPEND WebKit_UNIFIED_SOURCE_LIST_FILES
    "SourcesHaiku.txt"
)

# FIXME: WebCore should already have a dependency on PAL and WTF, but it seems
# this is not the case for the Haiku port. We explicitly add them here as a
# workaround. This should be investigated further.
#
# NOTE: This is currently disabled as it might cause build issues. If you
# are experiencing linking errors with PAL or WTF, you can try re-enabling
# this section.
#
# LIST(APPEND WebKit_LIBRARIES
#     WebCore PAL WTF
# )
LIST(APPEND WebKit_LIBRARIES
    ${LIBXML2_LIBRARIES}
    ${SQLITE_LIBRARIES}
    ${PNG_LIBRARY}
    ${JPEG_LIBRARY}
    ${CMAKE_DL_LIBS}
    be bnetapi GL shared translation tracker
)

INSTALL(FILES
    haiku/API/WebWindow.h
    haiku/API/WebViewConstants.h
    haiku/API/WebView.h
    haiku/API/WebSettings.h
    haiku/API/WebPage.h
    haiku/API/WebKitInfo.h
    haiku/API/WebFrame.h
    haiku/API/WebDownload.h
    DESTINATION develop/headers${CMAKE_HAIKU_SECONDARY_ARCH_SUBDIR}
    COMPONENT devel
)

set(WebKit_WEB_PREFERENCES_TEMPLATES
    ${WEBKIT_DIR}/haiku/Scripts/PreferencesTemplates/WebPreferencesDefinitions.h.erb
    ${WEBKIT_DIR}/haiku/Scripts/PreferencesTemplates/WebSettingsPrivateGenerated.cpp.erb
)

set(WebKit_WEB_PREFERENCES
    ${WTF_SCRIPTS_DIR}/Preferences/UnifiedWebPreferences.yaml
    ${WEBKIT_DIR}/haiku/HaikuWebPreferences.yaml
)

set_source_files_properties(${WebKit_WEB_PREFERENCES} PROPERTIES GENERATED TRUE)

add_custom_command(
    OUTPUT ${WebKit_DERIVED_SOURCES_DIR}/WebPreferencesDefinitions.h ${WebKit_DERIVED_SOURCES_DIR}/WebSettingsPrivateGenerated.cpp
    DEPENDS ${WebKit_WEB_PREFERENCES_TEMPLATES} ${WebKit_WEB_PREFERENCES} WTF_CopyPreferences
    COMMAND ${RUBY_EXECUTABLE} ${WTF_SCRIPTS_DIR}/GeneratePreferences.rb --frontend WebKit --outputDir "${WebKit_DERIVED_SOURCES_DIR}" --template ${WEBKIT_DIR}/haiku/Scripts/PreferencesTemplates/WebSettingsPrivateGenerated.cpp.erb --template ${WEBKIT_DIR}/haiku/Scripts/PreferencesTemplates/WebPreferencesDefinitions.h.erb ${WebKit_WEB_PREFERENCES}
    VERBATIM)

list(APPEND WebKit_SOURCES
    ${WebKit_DERIVED_SOURCES_DIR}/WebSettingsPrivateGenerated.cpp
)

list(APPEND WebKit_SOURCES ${WebKit_INCLUDES} ${WebKit_SOURCES_Classes} ${WebKit_SOURCES_WebCoreSupport})
