project('glwrap', 'cpp', default_options: ['warning_level=3', 'cpp_std=c++17'])

vcpkg = find_program('vcpkg', required : true)
run_command(vcpkg,
  'install',
  '--x-wait-for-lock',
  '--x-manifest-root=' + meson.current_source_dir(),
  check: true)

fmt_dep = dependency('fmt', include_type: 'system')
glm_dep = dependency('glm', include_type: 'system')
glad_dep = dependency('glad', fallback: ['glad', 'glad_dep'], include_type: 'system')

glw_src = [
  'src/buffer.cpp',
  'src/framebuffer.cpp',
  'src/imageformat.cpp',
  'src/log.cpp',
  'src/renderbuffer.cpp',
  'src/shader.cpp',
  'src/state.cpp',
  'src/texture.cpp',
  'src/utility.cpp',
  'src/vertexarray.cpp',
  'src/vertexformat.cpp',
]

glw_deps = [fmt_dep, glad_dep]
glw = static_library('glw', glw_src,
  include_directories: 'include/',
  dependencies: glw_deps)
glw_dep = declare_dependency(
  include_directories: include_directories('include/'),
  link_with: [glw],
  dependencies: glw_deps)

sdl2_dep = dependency('sdl2', include_type: 'system')
stb_dep = declare_dependency() # handled by vcpkg include directory

glwx_src = [
  'src/glwx/aabb.cpp',
  'src/glwx/buffers.cpp',
  'src/glwx/debug.cpp',
  'src/glwx/indexaccessor.cpp',
  'src/glwx/math.cpp',
  'src/glwx/mesh.cpp',
  'src/glwx/meshgen.cpp',
  'src/glwx/primitive.cpp',
  'src/glwx/rendertarget.cpp',
  'src/glwx/shader.cpp',
  'src/glwx/spriterenderer.cpp',
  'src/glwx/texture.cpp',
  'src/glwx/transform.cpp',
  'src/glwx/transform2d.cpp',
  'src/glwx/utility.cpp',
  'src/glwx/vertexaccessor.cpp',
  'src/glwx/window.cpp',
]

glwx_deps = [glw_dep, sdl2_dep, stb_dep]
glwx = static_library('glwx', glwx_src,
  include_directories: 'include/',
  dependencies: glwx_deps)
glwx_dep = declare_dependency(
  include_directories: include_directories('include/'),
  link_with: [glwx],
  dependencies: glwx_deps)

if not meson.is_subproject()
  executable('rendertarget', 'examples/rendertarget.cpp', dependencies: [glwx_dep])
  executable('mesh', 'examples/mesh.cpp', dependencies: [glwx_dep])
  executable('3d', 'examples/3d.cpp', dependencies: [glwx_dep])
endif
