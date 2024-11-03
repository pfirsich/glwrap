# 3.3 is required for SDL2 (CMP0058)
cmake_minimum_required(VERSION 3.3.0)

project(glwrap)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

add_compile_definitions(NOMINMAX) # Windows is trash

include(cmake/wall.cmake)

if(GLWRAP_ENABLE_ASAN)
  include(cmake/asan.cmake)
endif()

include(cmake/CPM.cmake)

CPMAddPackage(
  NAME fmt
  VERSION 11.0.2
  URL "https://github.com/fmtlib/fmt/releases/download/11.0.2/fmt-11.0.2.zip"
  URL_HASH MD5=c622dca45ec3fc95254c48370a9f7a1d
)

CPMAddPackage(
  NAME glm
  VERSION 1.0.1
  URL "https://github.com/g-truc/glm/archive/refs/tags/1.0.1.zip"
  URL_HASH MD5=78e541dadeee540ff9cedb5b618df821
)

CPMAddPackage(
  NAME SDL2
  VERSION 2.26.5
  URL "https://github.com/libsdl-org/SDL/releases/download/release-2.26.5/SDL2-2.26.5.zip"
  URL_HASH MD5=0664f3980570c4641128866e6c9f2e29
)

set(GLW_SRC
  buffer.cpp
  framebuffer.cpp
  imageformat.cpp
  log.cpp
  renderbuffer.cpp
  shader.cpp
  state.cpp
  texture.cpp
  utility.cpp
  vertexarray.cpp
  vertexformat.cpp
)
list(TRANSFORM GLW_SRC PREPEND src/)

add_library(glad "deps/glad/glad.c")
target_include_directories(glad SYSTEM PUBLIC deps/glad/include)
target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})

add_library(glw STATIC ${GLW_SRC})
target_include_directories(glw PUBLIC include/)
target_link_libraries(glw PUBLIC glad)
target_link_libraries(glw PUBLIC fmt::fmt)
target_link_libraries(glw PUBLIC glm::glm)
target_compile_definitions(glw PUBLIC GLM_ENABLE_EXPERIMENTAL) # for string_cast, quaternion and more

set_wall(glw)

set(GLWX_SRC
  aabb.cpp
  buffers.cpp
  debug.cpp
  indexaccessor.cpp
  math.cpp
  mesh.cpp
  meshgen.cpp
  primitive.cpp
  rendertarget.cpp
  shader.cpp
  spriterenderer.cpp
  texture.cpp
  transform.cpp
  transform2d.cpp
  utility.cpp
  vertexaccessor.cpp
  window.cpp
)
list(TRANSFORM GLWX_SRC PREPEND src/glwx/)

add_library(glwx STATIC ${GLWX_SRC})
target_include_directories(glwx PUBLIC include/)
target_include_directories(glwx SYSTEM PUBLIC deps/stb)
target_link_libraries(glwx PUBLIC glw)
target_link_libraries(glwx PUBLIC SDL2::SDL2)
target_compile_definitions(glwx PUBLIC SDL_MAIN_HANDLED) # don't override main()

set_wall(glwx)

option(GLWRAP_BUILD_EXAMPLES "Build examples" OFF)
if(GLWRAP_BUILD_EXAMPLES)
  message("Building examples")
  add_subdirectory(examples)
endif()
