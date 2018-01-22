#ifndef __TranslationUnit_h__
#define __TranslationUnit_h__

#include <memory>
#include <string>
#include <list>
#include <unordered_map>
#include <regex>

#include <dwarf.h>

class ObjectFileManager;

class TranslationUnit {
  private:
    std::string source_filename;
    Dwarf_Die cu_die;
    ObjectFileManager *mgr;
    std::unordered_map<std::string, std::string> includes;
    std::regex include_regex {"\\s*#include\\s*[<\"]([^\">]+)[>\"]"};
    bool initialized {false};

    void extract_includes(void);
    void extract_includes_1(Dwarf_Macro_Context mcontext, Dwarf_Unsigned number_of_ops);
    std::string get_header_string (std::string include_filename, int line_number);
    void get_include_paths(std::vector<char *> & flags);


  public:
    TranslationUnit(ObjectFileManager *mgr, std::string filename, Dwarf_Die die);
    ~TranslationUnit(void);

    void init(void);
    std::vector<char *> * get_compile_flags(void);

};

#endif