
add_library(badge_display INTERFACE)

target_sources(badge_display INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/framebuffer.c
        ${CMAKE_CURRENT_LIST_DIR}/framebuffer_micropython.c
        ${CMAKE_CURRENT_LIST_DIR}/assets.c
        ${CMAKE_CURRENT_LIST_DIR}/assets_micropython.c
        ${CMAKE_CURRENT_LIST_DIR}/assetList.c
        ${CMAKE_CURRENT_LIST_DIR}/assetList_micropython.c)

target_include_directories(badge_display INTERFACE ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/assets/)

target_link_libraries(usermod INTERFACE badge_display)