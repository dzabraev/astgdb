test_pro = executable('test-pro', 'pro.cc',
  include_directories: include_objutils,
  dependencies: [lib_objutils_dep, llvm_dep, clang_dep])

include_test_sample = [include_directories('p1'), include_directories('p2')]
test_sample = executable('sample', 'sample.cc',
  include_directories: include_test_sample)

test('path recover order', test_pro)