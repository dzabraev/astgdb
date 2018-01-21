#include <assert.h>

#include "LazyFile.h"

LazyFile::LazyFile (std::string filename) {
  this->filename = filename;
  exhausted = false;
  file = std::fstream(filename, std::fstream::in);
}

const std::string & LazyFile::operator[] (int line_number) {
  static std::string empty_string = "";
  if (read_before (line_number))
    return lines[line_number];
  else
    return empty_string;
}


bool LazyFile::read_before(int line_num) {
  assert (line_num>=0);
  if (line_num<n_lines_read)
    return true;
  if (exhausted)
    return false;
  std::string line;
  while (std::getline(file, line)) {
    if (ignore_line (line))
      continue;
    lines.push_back(line);
    n_lines_read++;
    if (line_num+1==n_lines_read)
      return true;
  }
  exhausted = true;
  return false;
}


bool LazyFile::ignore_line (std::string & line) {
  return false;
}

bool LazyHeaderFile::ignore_line (std::string & line) {
  std::smatch match;
  if(std::regex_search(line, match, regex_line_directive)) {
    //ignore this directives #line 1 "test"
    return true;
  }
  return false;
}


