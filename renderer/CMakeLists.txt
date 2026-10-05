file(GLOB_RECURSE RENDERER_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(renderer STATIC ${RENDERER_SOURCES})

target_include_directories(renderer
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND_CLIENT REQUIRED IMPORTED_TARGET wayland-client)

target_link_libraries(renderer
    PUBLIC  PkgConfig::WAYLAND_CLIENT
    PRIVATE lib project_warnings charpaper_headers
)
