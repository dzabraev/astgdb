#include <list>
#include <memory>
#include <stack>
#include <unordered_set>
#include <libdwarf.h>
#include <dwarf.h>
#include <tuple>
#include <stdio.h>

#include "ObjectFileManager.h"
#include "TranslationUnit.h"
#include "Diagnostic.h"

#include <boost/format.hpp>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/LLVM.h"
#include "clang/Frontend/ASTUnit.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Lex/PreprocessorOptions.h"

using namespace clang;

TranslationUnit::TranslationUnit(ObjectFileManager *mgr,
    std::string filename, Dwarf_Die cu_die, astgdb::Diagnostic *diag) {
  this->mgr = mgr;
  this->source_filename = filename;
  this->cu_die = cu_die;
  this->diag = diag;
}

TranslationUnit::~TranslationUnit(void) {
  dwarf_dealloc(mgr->get_dwarf_dbg(), cu_die, DW_DLA_DIE);
  for (auto pair : predefined_constants) {
    if (pair.first)
      free (pair.first);
    if (pair.second)
      free (pair.second);
  }
  for (auto key : predefined_constants_flags) {
    free (key);
  }
  for (auto flag : predefined_files_flags) {
    free (flag);
  }
}

void TranslationUnit::init(void) {
  if (initialized)
    return;
  extract_macro_info();
  initialized=true;
}

void
TranslationUnit::extract_macro_info_1(
    Dwarf_Macro_Context mcontext,
    Dwarf_Unsigned number_of_ops)
{
  int res;
  Dwarf_Error err = 0;
  std::stack<unsigned> header_stack;

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
        diag->warning(boost::format("ERROR from  dwarf_get_macro_op(): %s") %
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
          diag->warning(boost::format("ERROR from dwarf_get_macro_startend_file(): %s") %
              dwarf_errmsg (err));
          return;
        }
        if (macro_string) {
          fs::path include_filename = fs::weakly_canonical (fs::path (macro_string));
          std::string sp = include_filename.string();
          if (line_number>0 && !header_stack.empty()) {
            unsigned parent_file_uid = header_stack.top();
            //auto triplet = std::tuple<unsigned,unsigned,std::string>();
            includes.push_back({parent_file_uid,line_number,include_filename.string()});
          }
          else if (line_number==0 && include_filename.string()!=source_filename) {
            // predefined file.
            predefined_files.push_back(include_filename.string());
            char *flag;
            asprintf(&flag,"-include%s",include_filename.string().c_str());
            predefined_files_flags.push_back(flag);
          }
          header_stack.push (mgr->get_fileMgr()->getFile(StringRef(include_filename.string()))->getUID());
        }
        break;
      }
      case DW_MACRO_end_file: {
        header_stack.pop();
        break;
      }
      case DW_MACRO_import: {
        if (header_stack.empty()) {
          // predefined constants and names
          // We should process DW_MACRO_import
          // directive only when we do not enter into file.
          // Because in this case this imformation will perform
          // in ast builder.
          res = dwarf_get_macro_import(mcontext,
            k,&offset,&err);
          if (res != DW_DLV_OK) {
            diag->warning(boost::format("ERROR from dwarf_get_macro_import(): %s") %
              dwarf_errmsg (err));
            continue;
          }
          process_macro_import(offset);
        }
        break;
      }
      case DW_MACRO_define_strp:
      case DW_MACRO_define_strx:
      case DW_MACRO_define_sup: {
        res = dwarf_get_macro_defundef(mcontext,
            k,
            &line_number,
            &index,
            &offset,
            &forms_count,
            &macro_string,
            &err);
        if (res != DW_DLV_OK) {
          diag->warning(boost::format("ERROR from sup dwarf_get_macro_defundef(): %s") %
            dwarf_errmsg (err));
          continue;
        }
        if (line_number==0) {
          // predefined constants and names such -Dx=y -DNAME
          int idx=-1;
          const char *p=macro_string;
          while (*p) {
            if (*p==' ') {
              idx = p-macro_string;
              p++;
            }
          }
          char *name;
          char *val = 0;
          char *key;
          if (idx==-1) {
            name = strdup(macro_string);
            asprintf(&key,"-D%s",name);
          }
          else {
            name = strndup(macro_string, idx);
            val = strdup(macro_string+idx+1);
            asprintf(&key,"-D%s=%s",name,val);
          }
          predefined_constants.push_back({name,val});
          predefined_constants_flags.push_back(key);
        }
      }
    }
  }
}


void TranslationUnit::extract_macro_info (void) {
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
    extract_macro_info_1 (mcontext, number_of_ops);
    dwarf_dealloc_macro_context(mcontext);
  }
  else {
    if (err)
      diag->warning(boost::format("dwarf_get_macro_context: %s\n") % dwarf_errmsg (err));
    return;
  }
}

void TranslationUnit::process_macro_import(Dwarf_Unsigned offset) {
  int lres = 0;
  Dwarf_Unsigned version = 0;
  Dwarf_Macro_Context macro_context = 0;
  Dwarf_Unsigned number_of_ops = 0;
  Dwarf_Unsigned ops_total_byte_len = 0;
  Dwarf_Error err = 0;

  lres = dwarf_get_macro_context_by_offset(cu_die,
    offset,
    &version,&macro_context,
    &number_of_ops,
    &ops_total_byte_len,
    &err);

  if(lres == DW_DLV_NO_ENTRY) {
    return;
  }

  if(lres == DW_DLV_ERROR) {
    diag->warning(boost::format("Unable to dwarf_get_macro_context() %s\n") % dwarf_errmsg (err));
    return;
  }

  for (int idx=0; idx<number_of_ops;idx++) {
    
  }

}


ASTUnit * TranslationUnit::produce_ast(void) {
  IntrusiveRefCntPtr<DiagnosticsEngine>
    Diags(CompilerInstance::createDiagnostics(new DiagnosticOptions));

  std::unique_ptr<std::vector<const char *>> Args(
      new std::vector<const char *>());
  Args->push_back("-detailed-preprocessing-record");
  Args->push_back("-fsyntax-only");
  Args->push_back(source_filename.c_str());

  std::cout<<boost::format("Total number of names = %s") % predefined_constants_flags.size()<<std::endl;
  std::cout<<boost::format("Total number of include files = %s") % includes.size()<<std::endl;

  for (auto flag : predefined_constants_flags) {
    Args->push_back(flag);
    std::cout<<flag<<std::endl;
  }

  for (auto flag : predefined_files_flags) {
    Args->push_back(flag);
    std::cout<<flag<<std::endl;
  }

  FileSystemOptions FSOpts;

  CompilerInvocation * ptrCI = new CompilerInvocation;
  std::shared_ptr<CompilerInvocation> CI(ptrCI);

  CompilerInvocation::CreateFromArgs(*CI,Args->data(), Args->data() + Args->size(), *Diags);

  for (auto && [parentUID, parentLine, inclAbsPath ]: includes) {
    CI->getPreprocessorOpts().addInclMapping(parentUID, parentLine, inclAbsPath);
  }

  std::shared_ptr<PCHContainerOperations> pch_container(new PCHContainerOperations);

  ast = ASTUnit::LoadFromCompilerInvocation(
    CI, pch_container, Diags, mgr->get_fileMgr()).release();

  return ast;
}



