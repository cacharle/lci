project(
  'lci',
  'c',
  default_options : ['c_std=c99'],
  # c_args : ['Wall', 'Wextra']
)
# b_coverage=true
# MAP_ANONYMOUS not defined when using meson
add_global_arguments('-g', language : 'c')
# add_global_arguments('--coverage', language : 'c')
# incdir = include_directories('include')
subdir('src')
cc = meson.get_compiler('c')
readline_dep = cc.find_library('readline', required : true)
executable('lci', sources, dependencies : readline_dep)
subdir('test')
