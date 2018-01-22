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
        for (auto flag : *flags) {
          std::cout<<flag<<std::endl;
        }
        return 0;
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
  std::string unit_name = path + std::string(".cc");
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