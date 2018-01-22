#include <list>
#include <memory>
#include <stack>
#include <unordered_set>
#include <libdwarf.h>
#include <dwarf.h>

#include "ObjectFileManager.h"
#include "TranslationUnit.h"
#include "LazyFile.h"

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;


TranslationUnit::TranslationUnit(ObjectFileManager *mgr, std::string filename, Dwarf_Die cu_die) {
  this->mgr = mgr;
  this->source_filename = filename;
  this->cu_die = cu_die;
}

TranslationUnit::~TranslationUnit(void) {
  dwarf_dealloc(mgr->get_dwarf_dbg(), cu_die, DW_DLA_DIE);
}

std::vector<char *> *TranslationUnit::get_compile_flags(void) {
  std::vector<char *> *flags = new std::vector<char *>;
  get_include_paths(*flags);
  return flags;
}


void TranslationUnit::get_include_paths(std::vector<char *> & flags) {
  std::unordered_set<std::string> paths;
  for (auto && [ incl, fullpath ]: includes) {
    size_t len = fullpath.size(),
           incl_len = incl.size();
    std::string && path = fullpath.substr(len-incl_len, incl_len);
    if (path==incl) {
      paths.insert(fullpath.substr(0,len-incl_len));
    }
    else {
      mgr->warning(boost::format("Bad suffix: suffix=%s path=%s") % incl % fullpath);
      continue;
    }
  }
  for (auto path : paths) {
    char *flg;
    asprintf(&flg, "-I%s", path.c_str());
    flags.push_back(flg);
  }
}

void TranslationUnit::init(void) {
  if (initialized)
    return;
  extract_includes();
  initialized=true;
}

void
TranslationUnit::extract_includes_1(
    Dwarf_Macro_Context mcontext,
    Dwarf_Unsigned number_of_ops)
{
  int res;
  Dwarf_Error err = 0;
  std::stack<fs::path> header_stack;

  for (Dwarf_Unsigned k = 0; k < number_of_ops; ++k) {
    Dwarf_Unsigned  section_offset = 0;
    Dwarf_Half      macro_operator = 0;
    Dwarf_Half      forms_count = 0;
    const Dwarf_Small *formcode_array = 0;
    Dwarf_Unsigned  line_number = 0;
    Dwarf_Unsigned  index = 0;
    Dwarf_Unsigned  offset =0;
    const char    * macro_string =0;

    res = dwarf_get_macro_op(mcontext,
        k, &section_offset,&macro_operator,
        &forms_count, &formcode_array,&err);
    if (res != DW_DLV_OK) {
        mgr->warning(boost::format("ERROR from  dwarf_get_macro_op(): %s") %
            dwarf_errmsg (err));
        return;
    }

    switch(macro_operator) {
      case DW_MACRO_start_file: {
        res = dwarf_get_macro_startend_file(mcontext,
          k,&line_number,
          &index,
          &macro_string,&err);
        if (res != DW_DLV_OK) {
          mgr->warning(boost::format("ERROR from dwarf_get_macro_startend_file(): %s") %
              dwarf_errmsg (err));
          return;
        }
        if (macro_string) {
          fs::path include_filename = fs::weakly_canonical (fs::path (macro_string));
          std::string sp = include_filename.string();
          if (line_number>0) {
            fs::path parent_filename = header_stack.top();
            std::string header_string = get_header_string (parent_filename.string(), line_number-1);
            if (!header_string.empty()) {
              auto search = includes.find(header_string);
              if(search==includes.end()) {
                includes.insert({header_string, sp});
              }
            }
          }
          header_stack.push (include_filename);
        }
        break;
      }
      case DW_MACRO_end_file: {
        header_stack.pop();
        break;
      }
      case DW_MACRO_import: {
        res = dwarf_get_macro_import(mcontext,
            k,&offset,&err);
        if (res != DW_DLV_OK) {
          mgr->warning(boost::format("ERROR from dwarf_get_macro_import(): %s") %
              dwarf_errmsg (err));
          return;
        }
        break;
      }
    }
  }
}

std::string TranslationUnit::get_header_string(
  std::string include_filename, int line_number)
{
  LazyHeaderFile *f=mgr->get_header(include_filename);
  std::smatch include_match;
  if(std::regex_search(f[0][line_number], include_match, include_regex)) {
    return include_match[1];
  }
  return std::string("");
}

void TranslationUnit::extract_includes (void) {
  Dwarf_Unsigned version = 0;
  Dwarf_Macro_Context mcontext = 0;
  Dwarf_Unsigned macro_unit_offset = 0;
  Dwarf_Unsigned number_of_ops = 0;
  Dwarf_Unsigned ops_total_byte_len = 0;
  Dwarf_Error err;
  int res;

  res = dwarf_get_macro_context (cu_die,
     &version,
     &mcontext,
     &macro_unit_offset,
     &number_of_ops,
     &ops_total_byte_len,
     &err);

  if (res==DW_DLV_OK) {
    extract_includes_1 (mcontext, number_of_ops);
    dwarf_dealloc_macro_context(mcontext);
  }
  else {
    if (err)
      mgr->warning(boost::format("dwarf_get_macro_context: %s\n") % dwarf_errmsg (err));
    return;
  }
}
