project('astgdb', 'cpp',
  version : '0.0.1',
  license : 'MIT',
  default_options : ['cpp_std=c++1z'])

add_global_arguments(['-g3','-O0'], language : 'cpp')

libelf_dep = dependency('libelf')
boost_dep = dependency('boost')

include_objutils = include_directories('src/objutils/')

lib_objutils = static_library('objutils', [
  'src/objutils/Diagnostic.cc',
  'src/objutils/LazyFile.cc',
  'src/objutils/ObjectFileManager.cc',
  'src/objutils/TranslationUnit.cc'],
  include_directories: include_objutils)

executable('main', 'src/main.cc',
  include_directories: include_objutils,
  link_with: lib_objutils)

subdir('tests')
