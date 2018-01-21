#include <stdio.h>
#include <iostream>
#include <vector>
#include <memory>
#include "ObjectFileManager.h"
#include "TranslationUnit.h"

int main(int argc, char **argv) {
  ObjectFileManager mgr(argc==1 ? "main" : argv[1]);
  if (mgr.is_initialized_correctly()) {
    TranslationUnit *tu = mgr.get_translation_unit("/home/dza/source/astgdb/src/LazyFile.cc");
    printf("p=%p\n",tu);
    std::vector<char *> *flags = tu->get_compile_flags();
    for (auto flag : *flags) {
      std::cout<<flag<<std::endl;
    }
  }
  else {
    std::cerr<<"Initialization failed"<<std::endl;
  }
}