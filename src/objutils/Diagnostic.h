#ifndef __diagnostic_utils_h__
#define __diagnostic_utils_h__

#include <string>
#include <tuple>
#include <iostream>
#include <vector>
#include <boost/format.hpp>

namespace astgdb {

enum class DiagType {
  NOTSET   = 0,
  DEBUG    = 10,
  INFO     = 20,
  WARNING  = 30,
  ERROR    = 40,
  CRITICAL = 50,
};

std::ostream& operator<<(std::ostream& os, DiagType type);

#define MSG_NOTSET(msg)     diag_msg_append(__FILE__, __LINE__, DiagType::NOTSET, msg)
#define MSG_DEBUG(msg)      diag_msg_append(__FILE__, __LINE__, DiagType::DEBUG, msg)
#define MSG_INFO(msg)       diag_msg_append(__FILE__, __LINE__, DiagType::INFO, msg)
#define MSG_WARNING(msg)    diag_msg_append(__FILE__, __LINE__, DiagType::WARNING, msg)
#define MSG_ERROR(msg)      diag_msg_append(__FILE__, __LINE__, DiagType::ERROR, msg)
#define MSG_CRITICAL(msg)   diag_msg_append(__FILE__, __LINE__, DiagType::CRITICAL, msg)

typedef std::tuple<std::string, const char *, int> DiagMsg;

class Diagnostic {
  public:
    std::vector<DiagMsg> diag_queue;
    int verbose {0};

    void diag_msg_append(const char *filename, int line, DiagType type, std::string msg);
    void warning(std::string msg, const char *filename=NULL, int line=-1);
    void warning(const char * msg, const char *filename=NULL, int line=-1);
    void warning(boost::basic_format<char>& msg, const char *filename=NULL, int line=-1);
};

}
#endif