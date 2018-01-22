#include <stdio.h>
#include <iostream>
#include <vector>
#include <memory>
#include "ObjectFileManager.h"
#include "TranslationUnit.h"

int main(int argc, char **argv) {
  ObjectFileManager mgr(argc>1 ? argv[1] : "main");
  const char *unit = argc>2 ? argv[2] : "/home/dza/source/astgdb/src/objutils/LazyFile.cc";
  if (mgr.is_initialized_correctly()) {
    TranslationUnit *tu = mgr.get_translation_unit(unit);
    printf("p=%p\n",tu);
    if (tu) {
      std::vector<char *> *flags = tu->get_compile_flags();
      for (auto flag : *flags) {
        std::cout<<flag<<std::endl;
      }
    }
  }
  else {
    std::cerr<<"Initialization failed"<<std::endl;
  }
}