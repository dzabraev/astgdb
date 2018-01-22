project('astgdb', 'cpp',
  version : '0.0.1',
  license : 'MIT',
  default_options : ['cpp_std=c++1z'])

libelf = dependency('libelf')

incdir = include_directories('src/objutils/')

srcs = ['src/main.cc',
  'src/objutils/Diagnostic.cc',
  'src/objutils/LazyFile.cc',
  'src/objutils/ObjectFileManager.cc',
  'src/objutils/TranslationUnit.cc']

executable('main', srcs, include_directories : incdir)