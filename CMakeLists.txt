cmake_minimum_required(VERSION 3.0.0)

project(glwrap)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

include(cmake/wall.cmake)

if (GLWRAP_ENABLE_ASAN)
  include(cmake/asan.cmake)
endif()

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

list(APPEND GLW_SRC deps/glad/glad.c)

find_package(fmt REQUIRED)

add_library(glw STATIC ${GLW_SRC})
target_include_directories(glw PUBLIC include)
target_include_directories(glw SYSTEM PUBLIC deps/glm/include)
target_include_directories(glw SYSTEM PUBLIC deps/glad/include)
target_link_libraries(glw fmt::fmt)
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
  texture.cpp
  transform.cpp
  utility.cpp
  vertexaccessor.cpp
  window.cpp
)
list(TRANSFORM GLWX_SRC PREPEND src/glwx/)

find_package(SDL2 REQUIRED)

add_library(glwx STATIC ${GLWX_SRC})
target_include_directories(glwx PUBLIC include/glwx)
target_include_directories(glwx SYSTEM PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(glwx SYSTEM PUBLIC deps/stb)
# CMAKE_DL_LIBS is for glad
target_link_libraries(glwx glw ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS})
set_wall(glwx)

if (GLWRAP_BUILD_EXAMPLES)
  message("Building examples")
  add_subdirectory(examples)
endif()
