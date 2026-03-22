#ifndef Plaid_Institution_hpp
#define Plaid_Institution_hpp

#include <string>
#include <vector>

namespace premia {
namespace plaid {

struct Institution {
  std::string institution_id;
  std::string name;
  std::vector<std::string> products;  // auth, transactions, balance, etc.
  std::string logo;  // base64-encoded logo image, may be empty
  std::string primary_color;
  std::string url;
};

}  // namespace plaid
}  // namespace premia

#endif  // Plaid_Institution_hpp
