add_library(charpaper_headers INTERFACE)
target_include_directories(charpaper_headers INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/headers
)

add_subdirectory(lib)
add_subdirectory(wayland_renderer)
add_subdirectory(daemon)
