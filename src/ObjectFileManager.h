#ifndef __ObjectFileManager_h__
#define __ObjectFileManager_h__

#include <string>
#include <memory>
#include <unordered_map>
#include <libdwarf.h>

#include "Diagnostic.h"

class LazyHeaderFile;
class TranslationUnit;
struct Elf;

class ObjectFileManager: public Diagnostic {
  public:
    TranslationUnit *get_translation_unit (std::string filename);
    LazyHeaderFile *get_header(std::string filename);
    Dwarf_Debug get_dwarf_dbg(void) {return dbg;}
    bool is_initialized_correctly(void) {return initialized_ok;}

    ObjectFileManager(const char *filename);
    ObjectFileManager(void *data, size_t n);
    ~ObjectFileManager (void);

  private:
    std::unordered_map<std::string,LazyHeaderFile *> headers;
    std::unordered_map<std::string,TranslationUnit *> translation_units;
    Dwarf_Debug dbg;
    Elf *elf;
    bool initialized_ok {false};

    void init(void *data, size_t n);
    bool init_dwarf(void *data, size_t n);
    bool extract_compilation_units(void);
};


#endif