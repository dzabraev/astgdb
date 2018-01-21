#include <clang-c/Index.h>
#include <iostream>
#include "llvm/ADT/StringRef.h"


void print_loc(void (*fn)(CXSourceLocation, CXFile *, unsigned *, unsigned *, unsigned *), CXCursor cursor) {
  CXCursorKind kind = clang_getCursorKind(cursor);
  CXSourceRange range = clang_getCursorExtent(cursor);
  CXSourceLocation  range_start = clang_getRangeStart (range);
  CXSourceLocation  range_end = clang_getRangeEnd (range);
  CXFile file_start, file_end;
  unsigned line_start, line_end;
  unsigned column_start, column_end;
  auto cursorName = clang_getCursorDisplayName(cursor);

  fn(range_start, &file_start, &line_start, &column_start, nullptr);
  fn(range_end, &file_end, &line_end, &column_end, nullptr);


  auto file_name_start = clang_getFileName(file_start);
  auto file_name_end = clang_getFileName(file_end);
  std::cout
    << clang_getCString (clang_getCursorKindSpelling (kind)) << " "
    << clang_getCString(cursorName) << " "
    << "start=" << clang_getCString(file_name_start) << ": " << line_start << ":" << column_start << "  "
    << "end=" << clang_getCString(file_name_end) << ": " << line_end << ":" << column_end << "  "
    << std::endl;


  clang_disposeString(file_name_start);
  clang_disposeString(file_name_end);
  clang_disposeString(cursorName);
}

CXChildVisitResult visitor2(CXCursor cursor, CXCursor, CXClientData) {
/*
  CXCursorKind kind = clang_getCursorKind(cursor);
  CXSourceRange range = clang_getCursorExtent(cursor);
  CXSourceLocation  range_start = clang_getRangeStart (range);
  CXSourceLocation  range_end = clang_getRangeEnd (range);
  CXFile file_start, file_end;
  unsigned line_start, line_end;
  unsigned column_start, column_end;
  auto cursorName = clang_getCursorDisplayName(cursor);
  llvm::StringRef file_name_start, file_name_end;

  clang_getFileLocation(range_start, &file_start, &line_start, &column_start, nullptr);
  clang_getFileLocation(range_end, &file_end, &line_end, &column_end, nullptr);


  file_name_start = clang_getFileName(file_start);
  file_name_end = clang_getFileName(file_end);
  std::cout
    << clang_getCString (clang_getCursorKindSpelling (kind)) << " "
    << clang_getCString(cursorName) << " "
    << "start=" << clang_getCString(file_name_start) << ": " << line_start << ":" << column_start << "  "
    << "end=" << clang_getCString(file_name_end) << ": " << line_end << ":" << column_end << "  "
    << std::endl;


  clang_disposeString(file_name_start);
  clang_disposeString(file_name_end);
*/
  print_loc(clang_getFileLocation, cursor);
  print_loc(clang_getExpansionLocation, cursor);
  print_loc(clang_getSpellingLocation, cursor);

  //clang_disposeString(cursorName);

  return CXChildVisit_Recurse;
}

CXChildVisitResult visitor(CXCursor cursor, CXCursor, CXClientData) {
  CXCursorKind kind = clang_getCursorKind(cursor);

  // Consider functions and methods
  if (kind == CXCursorKind::CXCursor_FunctionDecl ||
      kind == CXCursorKind::CXCursor_CXXMethod) {
    auto cursorName = clang_getCursorDisplayName(cursor);

    // Print if function/method starts with doSomething
    auto cursorNameStr = std::string(clang_getCString(cursorName));
    if (cursorNameStr.find("doSomething") == 0) {
      // Get the source locatino
      CXSourceRange range = clang_getCursorExtent(cursor);
      CXSourceLocation location = clang_getRangeStart(range);

      CXFile file;
      unsigned line;
      unsigned column;
      clang_getFileLocation(location, &file, &line, &column, nullptr);

      auto fileName = clang_getFileName(file);

      std::cout << "Found call to " << clang_getCString(cursorName) << " at "
                << line << ":" << column << " in " << clang_getCString(fileName)
                << std::endl;

      clang_disposeString(fileName);
    }

    clang_disposeString(cursorName);
  }

  return CXChildVisit_Recurse;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    return 1;
  }

  // Command line arguments required for parsing the TU
  constexpr const char *ARGUMENTS[] = {"-I./"};

  // Create an index with excludeDeclsFromPCH = 1, displayDiagnostics = 0
  CXIndex index = clang_createIndex(1, 0);

  CXTranslationUnit translationUnit;
  CXErrorCode err = clang_parseTranslationUnit2(
      index, argv[1], ARGUMENTS, std::extent<decltype(ARGUMENTS)>::value,
      nullptr, 0,
       //CXTranslationUnit_DetailedPreprocessingRecord |
       //CXTranslationUnit_IncludeBriefCommentsInCodeCompletion |
       CXTranslationUnit_None,
      &translationUnit);

  // Visit all the nodes in the AST
  CXCursor cursor = clang_getTranslationUnitCursor(translationUnit);
  clang_visitChildren(cursor, visitor2, 0);

  // Release memory
  clang_disposeTranslationUnit(translationUnit);
  clang_disposeIndex(index);

  return 0;
}
