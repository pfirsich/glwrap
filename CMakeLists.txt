cmake_minimum_required(VERSION 3.0.0)

project(glwrap)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${myl_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

# Enable address sanitizer in xcode so it can find the ASan library
# https://stackoverflow.com/a/53314315/5587737
set(CMAKE_XCODE_GENERATE_SCHEME ON)
set(CMAKE_XCODE_SCHEME_ADDRESS_SANITIZER ON)
set(CMAKE_XCODE_SCHEME_ADDRESS_SANITIZER_USE_AFTER_RETURN ON)

function(set_warn target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /WX)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic -Werror)
  endif()
endfunction()

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
target_include_directories(glw PUBLIC deps/glm/include)
target_include_directories(glw PUBLIC deps/glad/include)
target_link_libraries(glw fmt::fmt)
set_warn(glw)


set(GLWX_SRC
  debug.cpp
  buffers.cpp
  mesh.cpp
  rendertarget.cpp
  shader.cpp
  texture.cpp
  utility.cpp
  window.cpp
)
list(TRANSFORM GLWX_SRC PREPEND src/glwx/)

find_package(SDL2 REQUIRED)

add_library(glwx STATIC ${GLWX_SRC})
target_include_directories(glwx PUBLIC include/glwx)
target_include_directories(glwx PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(glwx PUBLIC deps/stb)
target_link_libraries(glwx glw ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS})
set_warn(glwx)


add_executable(glwtest src/test.cpp)
target_link_libraries(glwtest glwx)
set_warn(glwtest)
