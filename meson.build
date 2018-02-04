project('astgdb', 'cpp',
  version : '0.0.1',
  license : 'MIT',
  default_options : ['cpp_std=c++1z'])

add_global_arguments(['-g3','-O0'], language : 'cpp')

libelf_dep = dependency('libelf')
boost_dep = dependency('boost')
#llvm_dep = dependency('llvm')

clang_deps_ldflags = [
  '-lclangFrontendTool',
  '-lclangFrontend',
  '-lclangDriver',
  '-lclangSerialization',
  '-lclangCodeGen',
  '-lclangParse',
  '-lclangSema',
  '-lclangEdit',
  '-lclangAnalysis',
  '-lclangIndex',
  '-lclangRewrite',
  '-lclangAST',
  '-lclangLex',
  '-lclangBasic'
]
clang_dep = declare_dependency(link_args: clang_deps_ldflags)

subdir('src') # lib_objutils_dep
              # include_objutils

executable('main', 'src/main.cc',
  include_directories: include_objutils,
  dependencies: [
    lib_objutils_dep,
    #llvm_dep,
    clang_dep
  ],
)

subdir('tests')
