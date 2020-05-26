#
# CMake Packaging macros for OSKAR.
#

set(CPACK_PACKAGE_NAME "OSKAR")
set(CPACK_PACKAGE_VENDOR "University of Oxford")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_NAME})
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OSKAR-${OSKAR_VERSION}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "${OSKAR_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${OSKAR_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${OSKAR_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${OSKAR_VERSION}")
set(CPACK_PROJECT_CONFIG_FILE ${PROJECT_SOURCE_DIR}/cmake/oskar_cpack.cmake)
set(CPACK_SOURCE_IGNORE_FILES
    "/build/;/debug/;/dbg/;/release/;"
    "/.git/;/.idea/;/.settings/;"
    ".DS_Store;.project;.cproject;"
)
set(CPACK_SOURCE_GENERATOR "ZIP")

if (APPLE)
    set(CPACK_MONOLITHIC_INSTALL 1)

    # Bundle generator options.
    configure_file(${PROJECT_SOURCE_DIR}/cmake/Info.plist.in
        ${PROJECT_BINARY_DIR}/Info.plist @ONLY)
    set(CPACK_GENERATOR "Bundle")
    set(CPACK_BUNDLE_NAME "OSKAR")
    set(CPACK_BUNDLE_PLIST ${PROJECT_BINARY_DIR}/Info.plist)
    set(CPACK_BUNDLE_ICON ${PROJECT_SOURCE_DIR}/gui/icons/oskar.icns)
    set(CPACK_BUNDLE_STARTUP_COMMAND ${PROJECT_SOURCE_DIR}/cmake/OSKAR)
    set(CPACK_DMG_VOLUME_NAME "OSKAR-${OSKAR_VERSION}")
    set(CPACK_COMMAND_HDIUTIL ${PROJECT_SOURCE_DIR}/cmake/oskar_hdiutil.sh)
    set(CPACK_PACKAGE_ICON ${PROJECT_SOURCE_DIR}/gui/icons/oskar.icns)
elseif (WIN32)
    # Bundle MSVC runtime libraries.
    set(CMAKE_INSTALL_OPENMP_LIBRARIES ON)
    include(InstallRequiredSystemLibraries)

    # Set up NSIS installer.
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "OSKAR")
    set(CPACK_COMPONENTS_ALL applications runtime headers libraries)
    set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "Applications")
    set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "Run-time Libraries")
    set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Development Libraries")
    set(CPACK_COMPONENT_HEADERS_DISPLAY_NAME "C/C++ Headers")
    set(CPACK_COMPONENT_HEADERS_DEPENDS libraries)
    set(CPACK_COMPONENT_APPLICATIONS_DEPENDS runtime)
    set(CPACK_COMPONENT_LIBRARIES_DEPENDS runtime)
    set(CPACK_COMPONENT_APPLICATIONS_REQUIRED ON)
    set(CPACK_COMPONENT_RUNTIME_REQUIRED ON)
    if (TARGET oskar_app)
        set(CPACK_PACKAGE_EXECUTABLES "oskar;OSKAR")
        set(CPACK_CREATE_DESKTOP_LINKS "oskar")
        set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\oskar.exe")
    endif()
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    install(FILES ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
        DESTINATION ${OSKAR_BIN_INSTALL_DIR} COMPONENT runtime)
else()
    set(CPACK_MONOLITHIC_INSTALL 1)
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)

add_custom_target(dist
    COMMAND ${CMAKE_MAKE_PROGRAM} package_source
    COMMENT "Source package"
    VERBATIM)
