#ifndef UserPrincipals_hpp
#define UserPrincipals_hpp

#include <string>
#include <unordered_map>
#include <vector>

#include "premia.h"

namespace premia {
namespace tda {

class UserPrincipals {
 private:
  ArrayList<StringMap> accounts_array;
  StringMap account_data;
  StringMap streamer_info;

 public:
  UserPrincipals();

  void add_account_data(const StringMap& data);
  void set_account_data(const StringMap& data);
  void set_streamer_info(const StringMap& data);
  StringMap get_account_data_array(int i);

  String get_account_data(CRString key);
};

}  // namespace tda
}  // namespace premia

#endif