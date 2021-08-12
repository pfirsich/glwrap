if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
  set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
endif()

cmake_minimum_required(VERSION 3.0.0)

project(glwrap)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

add_compile_definitions(NOMINMAX) # Windows is trash

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

find_package(fmt CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_library(glw STATIC ${GLW_SRC})
target_include_directories(glw PUBLIC include)
target_include_directories(glw SYSTEM PUBLIC deps/glad/include)
target_link_libraries(glw PUBLIC fmt::fmt)
if (TARGET glm::glm) # new target name (Ubuntu 20.04)
  target_link_libraries(glw PUBLIC glm::glm)
else() # old target name (vcpkg)
  target_link_libraries(glw PUBLIC glm)
endif()
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
  transform2d.cpp
  utility.cpp
  vertexaccessor.cpp
  window.cpp
)
list(TRANSFORM GLWX_SRC PREPEND src/glwx/)

find_package(SDL2 CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb.h" HINTS deps/stb)

add_library(glwx STATIC ${GLWX_SRC})
target_include_directories(glwx PUBLIC include/glwx)
target_include_directories(glwx SYSTEM PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(glwx SYSTEM PUBLIC ${STB_INCLUDE_DIRS})
# CMAKE_DL_LIBS is for glad
target_link_libraries(glwx PUBLIC glw)
target_link_libraries(glwx PUBLIC ${CMAKE_DL_LIBS})
if(SDL2_LIBRARIES) # for older CMake files (Ubuntu 20.04 package)
  target_link_libraries(glwx PUBLIC ${SDL2_LIBRARIES})
else() # for newer CMake files (incl. vcpkg)
  target_link_libraries(glwx PUBLIC SDL2::SDL2 SDL2::SDL2main)
endif()
set_wall(glwx)

if (GLWRAP_BUILD_EXAMPLES)
  message("Building examples")
  add_subdirectory(examples)
endif()
