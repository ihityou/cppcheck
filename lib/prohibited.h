#ifndef prohibitedH
#define prohibitedH
#include "config.h"

#include <cstddef>
#include <cstdint>
#include <istream>
#include <list>
#include <set>
#include <string>
#include <utility>
#include <vector>

class CPPCHECKLIB prohibited
{
private:
  std::string name;

public:
  prohibited() {}
  virtual ~prohibited() {}
  std::string parseJsonFile(const char *filename);
  struct CPPCHECKLIB ProhibitedStr
  {
    std::vector<std::string> filenames;
    std::vector<std::string> containWords;
    std::vector<std::string> wholeWords;
  };
  std::vector<ProhibitedStr> mProhibitedList;
};

#endif
