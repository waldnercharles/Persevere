cmake_minimum_required(VERSION 3.6)

project(Persevere)

include_directories(./include)

add_library(sdl2 STATIC IMPORTED)
set_property(TARGET sdl2 PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2.lib)

add_library(sdl2_main STATIC IMPORTED)
set_property(TARGET sdl2_main PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2main.lib)

add_library(${PROJECT_NAME}-core SHARED src/game_core.cpp)
add_executable(${PROJECT_NAME} src/platform.cpp)

target_link_libraries(${PROJECT_NAME} sdl2 sdl2_main)

add_custom_command(TARGET ${PROJECT_NAME}-core PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E touch $<TARGET_FILE_DIR:${PROJECT_NAME}-core>/${PROJECT_NAME}-core.lock)

add_custom_command(TARGET ${PROJECT_NAME}-core POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove -f $<TARGET_FILE_DIR:${PROJECT_NAME}-core>//${PROJECT_NAME}-core.lock)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"${PROJECT_SOURCE_DIR}/lib/SDL2.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>)