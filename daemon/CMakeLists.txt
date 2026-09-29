file(GLOB_RECURSE DAEMON_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

add_library(daemon STATIC ${DAEMON_SOURCES})

target_include_directories(daemon
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(daemon PRIVATE renderer lib project_warnings)
