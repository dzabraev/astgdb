#ifndef __ObjectFileManager_h__
#define __ObjectFileManager_h__

#include <string>
#include <memory>
#include <unordered_map>
#include <libdwarf.h>

class TranslationUnit;
struct Elf;
class HeaderExtractor;
namespace astgdb {
  class Diagnostic;
}

namespace clang {
  class FileManager;
}

using namespace clang;

class ObjectFileManager {
  public:
    TranslationUnit *get_translation_unit (std::string filename);
    Dwarf_Debug get_dwarf_dbg(void) {return dbg;}
    bool is_initialized_correctly(void) {return initialized_ok;}

    ObjectFileManager(const char *filename, clang::FileManager *fileMgr, astgdb::Diagnostic *diag);
    ObjectFileManager(std::string s, clang::FileManager *fileMgr,
      astgdb::Diagnostic *diag) : ObjectFileManager(s.c_str(),
        fileMgr, diag) {};
    ~ObjectFileManager (void);

    FileManager * get_fileMgr(void) {return fileMgr;}
    std::unordered_map<std::string,TranslationUnit *> & get_translation_units(void) {
      return translation_units;
    }

  private:
    std::unordered_map<std::string,TranslationUnit *> translation_units;
    Dwarf_Debug dbg;
    Elf *elf;
    FileManager *fileMgr;
    astgdb::Diagnostic *diag;
    bool initialized_ok {false};

    void init(void *data, size_t n);
    bool init_dwarf(void *data, size_t n);
    bool extract_compilation_units(void);
};


#endif