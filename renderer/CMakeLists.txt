file(GLOB_RECURSE RENDERER_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

add_library(renderer STATIC ${RENDERER_SOURCES})

target_include_directories(renderer
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(renderer PRIVATE lib project_warnings)
