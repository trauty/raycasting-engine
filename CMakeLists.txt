cmake_minimum_required(VERSION 3.0.0)
project(raycasting-engine)

add_library(glfw STATIC IMPORTED) 
set_target_properties(glfw PROPERTIES IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/libglfw3.a")

add_executable(raycasting-engine 
src/main.c
src/glad.c
src/shader.c
)

target_link_options(raycasting-engine PRIVATE -mwindows)

target_link_libraries(raycasting-engine glfw)

target_include_directories(raycasting-engine PRIVATE 
${CMAKE_SOURCE_DIR}/include/glfw
${CMAKE_SOURCE_DIR}/include/glad
${CMAKE_SOURCE_DIR}/include/
)