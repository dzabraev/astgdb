#include <boost/format.hpp>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include "ObjectFileManager.h"
#include "TranslationUnit.h"


class TestPRO: public ObjectFileManager {
  public:
    TestPRO(std::string obj_name, std::string unit_name) : ObjectFileManager(obj_name) {
      this->unit_name = unit_name;
    }
    int test_path_recover_order(void) {
      TranslationUnit *tu = get_translation_unit(unit_name);
      if (tu) {
        std::vector<char *> *flags = tu->get_compile_flags();
        if (flags->size()!=2) {
          std::cout<<boost::format("number of paths must be 2, but it is %d") % flags->size()<<std::endl;
          for (auto flag : *flags) {
            std::cout<<flag<<std::endl;
          }
          return 1;
        }
        else {
          std::string sp1 = fs::weakly_canonical(fs::path("../tests/pro/p1")).string();
          std::string sp2 = fs::weakly_canonical(fs::path("../tests/pro/p2")).string();
          const char *exp_p1 = sp1.c_str();
          const char *exp_p2 = sp2.c_str();
          assert (strlen(flags[0][0])>2);
          assert (strlen(flags[0][1])>2);
          const char *p1 = flags[0][0]+2;
          const char *p2 = flags[0][1]+2;
          bool err=false;
          if (strncmp(p1,exp_p1,strlen(exp_p1)-1)) {
            std::cout<<boost::format("incorrect p1: %s!=%s") % p1 % exp_p1<<std::endl;
            err=true;
          }
          if (strncmp(p2,exp_p2,strlen(exp_p2)-1)) {
            std::cout<<boost::format("incorrect p2: %s!=%s") % p2 % exp_p2<<std::endl;
            err=true;
          }
          if (err)
            return 1;
          else
            return 0;
        }
      }
      else {
        std::cerr<<boost::format("can't extract %s") % unit_name <<std::endl;
        return 1;
      }
      return 0;
    };
  private:
    std::string unit_name;
};

int main(void) {
  std::string path = fs::weakly_canonical(fs::path("./tests/pro/sample")).string();
  std::string unit_name = fs::weakly_canonical(fs::path("../tests/pro/sample.cc")).string();
  TestPRO mgr(path, unit_name);
  if (mgr.is_initialized_correctly()) {
    return mgr.test_path_recover_order();
  }
  else {
    std::cerr<<"Initialization failed"<<std::endl;
    return 1;
  }
  return 0;
}