#include <iostream>
#include "Diagnostic.h"
#include <boost/format.hpp>

namespace astgdb {

std::ostream& operator<<(std::ostream& os, DiagType type)
{
    switch(type)
    {
        case DiagType::NOTSET     : os << "NOTSET";   break;
        case DiagType::INFO       : os << "INFO";     break;
        case DiagType::WARNING    : os << "WARNING";  break;
        case DiagType::ERROR      : os << "ERROR";    break;
        case DiagType::CRITICAL   : os << "CRITICAL"; break;
        default                   : os << int(type);
    }
    return os;
}

void Diagnostic::diag_msg_append(const char *filename, int line, DiagType type, std::string msg) {
  if (int(type)>=verbose) {
    if (filename) {
      std::cerr<<filename<<":"<<line;
    }
    std::cerr<<msg<<std::endl;
  }
  diag_queue.push_back({msg,filename,line});
}

void Diagnostic::warning(std::string msg, const char *filename /*NULL*/, int line /*-1*/) {
  diag_msg_append(filename,line,DiagType::WARNING,msg);
}

void Diagnostic::warning(const char *msg, const char *filename /*NULL*/, int line /*-1*/) {
  warning(std::string(msg),filename,line);
}

void Diagnostic::warning(boost::basic_format<char>& msg, const char *filename /*NULL*/, int line /*-1*/) {
  warning(msg.str(),filename,line);
}

}