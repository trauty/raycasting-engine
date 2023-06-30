cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

project("raycasting-engine")

add_executable(raycasting-engine main.c)

target_link_options(raycasting-engine PRIVATE ${SUBSYSTEM_LINKER_OPTIONS})

target_link_libraries(raycasting-engine ${CMAKE_SOURCE_DIR}/lib/libSDL2main.a ${CMAKE_SOURCE_DIR}/lib/libSDL2.dll.a ${CMAKE_SOURCE_DIR}/lib/libSDL2_image.dll.a)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET raycasting-engine PROPERTY C_STANDARD 11)
endif()

target_include_directories(raycasting-engine PRIVATE ${CMAKE_SOURCE_DIR}/include)