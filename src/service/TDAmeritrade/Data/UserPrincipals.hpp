#ifndef UserPrincipals_hpp
#define UserPrincipals_hpp

#include <string>
#include <unordered_map>
#include <vector>

namespace premia {
namespace tda {

class UserPrincipals {
 private:
  std::vector<std::unordered_map<std::string, std::string>> accounts_array;
  std::unordered_map<std::string, std::string> account_data;
  std::unordered_map<std::string, std::string> streamer_info;

 public:
  UserPrincipals();

  void add_account_data(
      const std::unordered_map<std::string, std::string>& data);
  void set_account_data(
      const std::unordered_map<std::string, std::string>& data);
  void set_streamer_info(
      const std::unordered_map<std::string, std::string>& data);
  std::unordered_map<std::string, std::string> get_account_data_array(int i);

  std::string get_account_data(const std::string& key);
};

}  // namespace tda
}  // namespace premia

#endif