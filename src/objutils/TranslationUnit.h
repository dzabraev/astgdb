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
    std::vector<std::pair<std::string, bool>> Macros; // pairs {isUndef, macro_string}

    ASTUnit *ast {nullptr};
    std::vector<std::string> PreIncludes; //filenames whose will be include before parsing main file

    void add_define(bool isUndef, const char *macro_string) {
      std::string macro(macro_string);
      if (!isUndef) {
        for (size_t idx=0;idx<macro.size();idx++) {
          if (macro[idx]==' ') {
            if (macro.substr(0,idx)==std::string("__has_include_next(STR)") //gcc
                || macro.substr(0,idx)==std::string("__has_include(STR)") //gcc
            ) {
              //this is builtin macro in clang, skip
              return;
            }
            macro[idx]='=';
            break;
          }
        }
      }
      Macros.push_back({macro,isUndef});
    };

    void add_predefined_file(std::string filename) {
      PreIncludes.push_back(filename);
    }

    void extract_macro_info(void);

    void extract_macro_info_1(
      std::stack<unsigned> & header_stack,
      bool by_offset,
      Dwarf_Unsigned offset);

    void extract_macro_info_2(
      std::stack<unsigned> & header_stack,
      Dwarf_Macro_Context macro_context,
      Dwarf_Unsigned number_of_ops);


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
