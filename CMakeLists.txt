cmake_minimum_required(VERSION 3.18)

set(BOREALIS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/library/borealis)
set(BOREALIS_LIBRARY ${BOREALIS_DIR}/library)

if(NOT EXISTS "${BOREALIS_LIBRARY}/cmake/commonOption.cmake")
    message(FATAL_ERROR "Borealis dependency missing. Run: ./scripts/bootstrap-deps.sh")
endif()

include(${BOREALIS_LIBRARY}/cmake/commonOption.cmake)
include(${BOREALIS_LIBRARY}/cmake/toolchain.cmake)

project(XuitchTV)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "5")
set(VERSION_PATCH "1")
set(PROJECT_AUTHOR "XuitchTV Project")
set(PROJECT_ICON ${CMAKE_CURRENT_SOURCE_DIR}/resources/icon/icon.jpg)
set(PROJECT_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources)

add_subdirectory(library)

# Network layer. switch-curl exposes the same CMake package on devkitPro.
find_package(CURL REQUIRED)

# Playback layer. Current Switch media clients resolve switch-libmpv through
# pkg-config when cross-compiling with devkitPro.
set(XUITCHTV_MPV_FOUND OFF)
if(PLATFORM_SWITCH)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(MPV REQUIRED mpv)
    set(XUITCHTV_MPV_FOUND ON)
    link_directories(${MPV_LIBRARY_DIRS})

    # XuitchTV renders libmpv directly through the Switch/deko3d backend.
    # Fail during configure with a useful message if the installed mpv package
    # lacks the Switch-specific render header instead of failing much later.
    find_path(XUITCHTV_MPV_DEKO3D_INCLUDE
        NAMES mpv/render_dk3d.h
        HINTS ${MPV_INCLUDE_DIRS}
    )
    if(NOT XUITCHTV_MPV_DEKO3D_INCLUDE)
        message(FATAL_ERROR
            "switch-libmpv was found, but mpv/render_dk3d.h is missing. "
            "Install a Switch libmpv build with the deko3d render backend.")
    endif()
elseif(PLATFORM_DESKTOP)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_search_module(MPV QUIET mpv)
        if(MPV_FOUND)
            set(XUITCHTV_MPV_FOUND ON)
            link_directories(${MPV_LIBRARY_DIRS})
        endif()
    endif()
endif()

file(GLOB_RECURSE MAIN_SRC CONFIGURE_DEPENDS app/source/*.cpp)
if(PLATFORM_SWITCH)
    # Borealis/libnx service initialization:
    # RomFS, sockets, fonts, network and system services.
    list(APPEND MAIN_SRC
        ${BOREALIS_LIBRARY}/lib/platforms/switch/switch_wrapper.c
    )
endif()
program_target(${PROJECT_NAME} "${MAIN_SRC}")
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)

target_include_directories(${PROJECT_NAME} PRIVATE
    app/include
    ${APP_PLATFORM_INCLUDE}
    ${MPV_INCLUDE_DIRS}
)
target_compile_options(${PROJECT_NAME} PRIVATE -ffunction-sections -fdata-sections ${APP_PLATFORM_OPTION})
target_link_libraries(${PROJECT_NAME} PRIVATE
    borealis
    CURL::libcurl
    ${MPV_LIBRARIES}
    ${APP_PLATFORM_LIB}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    APP_VERSION="${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
    XUITCHTV_HAS_MPV=$<BOOL:${XUITCHTV_MPV_FOUND}>
)

if(PLATFORM_SWITCH)
    add_custom_target(${PROJECT_NAME}.nro DEPENDS ${PROJECT_NAME}

        COMMAND ${NX_NACPTOOL_EXE}
            --create "XuitchTV" "${PROJECT_AUTHOR}"
            "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}"
            ${PROJECT_NAME}.nacp

        # Rebuild the RomFS resources directory from scratch.
        COMMAND ${CMAKE_COMMAND} -E rm -rf
            ${CMAKE_BINARY_DIR}/resources

        COMMAND ${CMAKE_COMMAND} -E make_directory
            ${CMAKE_BINARY_DIR}/resources

        # Borealis runtime resources:
        # i18n, fonts, Material Icons, base XML, etc.
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${BOREALIS_DIR}/resources
            ${CMAKE_BINARY_DIR}/resources

        # XuitchTV resources are copied second so our files take priority.
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_RESOURCES}
            ${CMAKE_BINARY_DIR}/resources

        COMMAND ${NX_ELF2NRO_EXE}
            ${PROJECT_NAME}.elf
            ${PROJECT_NAME}.nro
            --icon=${PROJECT_ICON}
            --nacp=${PROJECT_NAME}.nacp
            --romfsdir=${CMAKE_BINARY_DIR}/resources

        VERBATIM
    )
endif()
