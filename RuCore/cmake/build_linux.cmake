target_sources(
    RuCore
    PRIVATE
        "${CMAKE_SOURCE_DIR}/RuCore/source/new_window_Linux.c"
        "${CMAKE_SOURCE_DIR}/RuCore/source/app_loop_Linux.c"
        "${CMAKE_SOURCE_DIR}/RuCore/source/utils_Linux.c"
)

target_link_libraries(
    RuCore
    PUBLIC
        GL
)
