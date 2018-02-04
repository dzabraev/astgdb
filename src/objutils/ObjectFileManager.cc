#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <assert.h>
#include <string.h>

#include <libdwarf.h>
#include <dwarf.h>
#include <libelf.h>

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"
#include "clang/Basic/FileManager.h"

#include "ObjectFileManager.h"
#include "TranslationUnit.h"
#include "Diagnostic.h"

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;


TranslationUnit *
ObjectFileManager::get_translation_unit(std::string filename) {
  auto search = translation_units.find(filename);
  if(search!=translation_units.end()) {
    TranslationUnit *tu = search->second;
    tu->init();
    return tu;
  }
  else {
    return nullptr;
  }
}


ObjectFileManager::ObjectFileManager(const char *filename,
    clang::FileManager *fileMgr, astgdb::Diagnostic *diag) {
  this->fileMgr = fileMgr;
  this->diag = diag;
  const FileEntry * fileEntry = fileMgr->getFile(StringRef(filename));
  auto buf = fileMgr->getBufferForFile(fileEntry);
  if (!buf) {
    diag->warning(boost::format("can't read file %s %s") % filename % buf.getError().message());
    return;
  }

  size_t sz = (*buf)->getBufferSize();
  void *buf1 = malloc(sz);
  assert (buf1);
  memcpy(buf1,(*buf)->getBufferStart(), sz);
  init (buf1, sz);
}

ObjectFileManager::~ObjectFileManager (void) {
  Dwarf_Error err;

  if (!initialized_ok)
    return;

  for (auto item : translation_units) {
    delete item.second;
  }
  dwarf_finish (dbg, &err);
  elf_end (elf);
}


void ObjectFileManager::init (void *data, size_t n) {
  if (!init_dwarf (data, n))
    return;
  if (!extract_compilation_units ())
    return;
  initialized_ok = true;
}

bool ObjectFileManager::init_dwarf (void *data, size_t n) {
  Dwarf_Error err;
  int res;

  elf_version(EV_CURRENT);
  if ((elf = elf_memory((char *) data, n)) == NULL) {
    /*error*/
    diag->warning(boost::format("elf_memory(): %s\n") %
      elf_errmsg(elf_errno()));
    if (elf_version(EV_CURRENT) == EV_NONE) {
      diag->warning("ELF library too old");
      return false;
    }
  }
  res = dwarf_elf_init (elf, DW_DLC_READ, NULL, NULL, &dbg, &err);
  if (res != DW_DLV_OK) {
    diag->warning(boost::format("dwarf_init: %s") % dwarf_errmsg (err));
    return false;
  }
  return true;
}

bool
ObjectFileManager::extract_compilation_units(void) {
  int res;
  Dwarf_Error err;
  bool has_error = false;

  while (true) {
    Dwarf_Die cu_die = 0;

    Dwarf_Unsigned cu_header_length = 0;
    Dwarf_Unsigned abbrev_offset = 0;
    Dwarf_Half     address_size = 0;
    Dwarf_Half     version_stamp = 0;
    Dwarf_Half     offset_size = 0;
    Dwarf_Half     extension_size = 0;
    Dwarf_Sig8     signature;
    Dwarf_Unsigned typeoffset = 0;
    Dwarf_Unsigned next_cu_header = 0;
    Dwarf_Half     header_cu_type = DW_UT_compile;
    Dwarf_Bool     is_info = true;

    res = dwarf_next_cu_header_d(dbg,is_info,&cu_header_length,
        &version_stamp, &abbrev_offset,
        &address_size, &offset_size,
        &extension_size,&signature,
        &typeoffset, &next_cu_header,
        &header_cu_type,&err);


    if(res == DW_DLV_ERROR) {
        diag->warning(boost::format("Error in dwarf_next_cu_header: %s") % dwarf_errmsg(err));
        has_error = true;
        break;
    }
    if(res == DW_DLV_NO_ENTRY) {
        /* Done. */
        break;
    }

    res = dwarf_siblingof_b (dbg, NULL,is_info, &cu_die, &err);
    if (res != DW_DLV_OK) {
      diag->warning(boost::format("siblingof cu header %s") % dwarf_errmsg (err));
      has_error = true;
      break;
    }



    Dwarf_Attribute comp_name_attr = 0;
    char *comp_name_attr_str=0;
    res = dwarf_attr(cu_die, DW_AT_name, &comp_name_attr, &err);
    if (res != DW_DLV_OK) {
      diag->warning(boost::format("dwarf_attr: %s") % dwarf_errmsg (err));
      continue;
    }
    res = dwarf_formstring(comp_name_attr, &comp_name_attr_str, &err);
    if (res!=DW_DLV_OK) {
      diag->warning("dwarf_formstring failed");
      continue;
    }

    Dwarf_Attribute comp_dir_attr = 0;
    char *comp_dir_attr_str=0;
    res = dwarf_attr(cu_die, DW_AT_comp_dir, &comp_dir_attr, &err);
    if (res != DW_DLV_OK) {
      diag->warning(boost::format("dwarf_attr: %s") % dwarf_errmsg (err));
      continue;
    }
    res = dwarf_formstring(comp_dir_attr, &comp_dir_attr_str, &err);
    if (res!=DW_DLV_OK) {
      diag->warning("dwarf_formstring failed");
      continue;
    }

    fs::path comp_unit_path = comp_name_attr_str[0]=='/' ? fs::path (comp_name_attr_str) : //if path relative, make it abs
        fs::weakly_canonical (fs::path (comp_dir_attr_str) / fs::path (comp_name_attr_str));
    std::string spath = comp_unit_path.string();
    TranslationUnit *tu = new TranslationUnit(this, spath, cu_die, diag);
    translation_units.insert ({spath, tu});
  }
  return !has_error;
}
