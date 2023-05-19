project('glad', 'c')

glad = static_library('glad', 'glad.c',
  include_directories: 'include',
  c_args: ['-Wno-pedantic'])
glad_dep = declare_dependency(
  include_directories: include_directories('include'),
  link_with: [glad])
