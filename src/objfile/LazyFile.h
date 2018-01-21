#ifndef __LazyFile_h__
#define __LazyFile_h__

#include <string>
#include <regex>
#include <fstream>
#include <vector>

class LazyFile {
  public :
    std::string filename;
    LazyFile (std::string filename);
    const std::string & operator[] (int line_number);

  private :
    bool exhausted;
    std::vector<std::string> lines;
    int n_lines_read {0};
    std::fstream file;

    virtual bool ignore_line (std::string & line);
    bool read_before(int line_num);
};

class LazyHeaderFile: public LazyFile {
  public :
    LazyHeaderFile (std::string filename) : LazyFile(filename) {};
  private :
    std::regex regex_line_directive {"\\s*#\\s*line"};
    virtual bool ignore_line (std::string & line);
};

#endif
