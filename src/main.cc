#include <stdio.h>
#include <iostream>
#include <vector>
#include <memory>

#include "clang/Basic/FileManager.h"
#include "clang/Basic/FileSystemOptions.h"

#include "ObjectFileManager.h"
#include "TranslationUnit.h"
#include "Diagnostic.h"

using namespace clang;

int main(int argc, char **argv) {
  FileSystemOptions FSOpts;
  astgdb::Diagnostic *diag = new astgdb::Diagnostic;
  FileManager *fileMgr = new FileManager(FSOpts);
  const char *filename = argc>1 ? argv[1] : "main";

  ObjectFileManager mgr(filename,fileMgr,diag);
  const char *unit = argc>2 ? argv[2] : "/home/dza/source/astgdb/src/objutils/LazyFile.cc";
  if (mgr.is_initialized_correctly()) {
    TranslationUnit *tu = mgr.get_translation_unit(unit);
    printf("p=%p\n",tu);
    if (tu) {
      tu->get_ast();
//      const std::vector<std::string> &flags = tu->get_compile_flags();
//      for (const auto flag : flags) {
//        std::cout<<flag<<std::endl;
//      }
    }
    else {
      for (auto && [key, tu1] : mgr.get_translation_units()) {
        std::cout<<tu1->get_source_filename()<<std::endl;
      }
    }
  }
  else {
    std::cerr<<"Initialization failed"<<std::endl;
  }
}