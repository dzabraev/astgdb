include_objutils = include_directories('./')

lib_objutils = static_library('objutils', [
  'Diagnostic.cc',
  'ObjectFileManager.cc',
  'TranslationUnit.cc'],
  include_directories: include_objutils)

lib_objutils_dep = declare_dependency(link_with : lib_objutils)
