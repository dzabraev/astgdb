test_pro = executable('test-pro', 'pro.cc',
  include_directories: include_objutils,
  link_with: lib_objutils)

include_test_sample = include_directories('p2', 'p1')
test_sample = executable('sample', 'sample.cc',
  include_directories: include_test_sample)

test('path recover order', test_pro)