cmake_minimum_required(VERSION 3.12)

project("sdl2test")

add_executable(sdl2test main.c)

target_link_libraries(sdl2test ${CMAKE_SOURCE_DIR}/lib/libSDL2main.a ${CMAKE_SOURCE_DIR}/lib/libSDL2.dll.a)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET sdl2test PROPERTY CXX_STANDARD 20)
endif()

target_include_directories(sdl2test PRIVATE ${CMAKE_SOURCE_DIR}/include)