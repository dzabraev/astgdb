#ifndef __TranslationUnit_h__
#define __TranslationUnit_h__

#include <memory>
#include <string>
#include <list>
#include <regex>
#include <vector>
#include <tuple>
#include <dwarf.h>
#include "clang/Frontend/ASTUnit.h"

class ObjectFileManager;

namespace astgdb {
  class Diagnostic;
}

using namespace clang;

class TranslationUnit {
  friend ObjectFileManager;

  private:
    std::string source_filename;
    Dwarf_Die cu_die;
    ObjectFileManager *mgr;
    std::vector<std::tuple<unsigned,unsigned,std::string>> includes; /// fileUID,line_num, abspath to header file
    bool initialized {false};
    astgdb::Diagnostic *diag;
    std::vector<std::pair<char *, char *>> predefined_constants; //-DNAME=123 --> (NAME,123)
    std::vector<char *> predefined_constants_flags; //same as predefined_constants but in joined form -DNAME=123
    ASTUnit *ast {nullptr};
    std::vector<char *> predefined_files_flags; //-include=FILE
    std::vector<std::string> predefined_files; //filenames whose will be include before parsing main file


    void extract_macro_info(void);
    void extract_macro_info_1(Dwarf_Macro_Context mcontext, Dwarf_Unsigned number_of_ops);

    ASTUnit * produce_ast(void);

  public:
    std::string & get_source_filename(void) {
      return source_filename;
    }

    ASTUnit * get_ast(void) {
      if (!ast)
        ast = produce_ast();
      return ast;
    }

    TranslationUnit(ObjectFileManager *mgr, std::string filename, Dwarf_Die die, astgdb::Diagnostic *diag);
    ~TranslationUnit(void);

    void init(void);
};

#endif
