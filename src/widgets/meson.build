include_widgets = include_directories('./')

lib_widgets = static_library('widgets', [
  'SourceFileAstVisitor.cc',
  'SrcView.cc'],
  include_directories: [
    include_widgets,
  ])

lib_widgets_dep = declare_dependency(link_with : [lib_widgets])
