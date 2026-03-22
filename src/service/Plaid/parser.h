#ifndef Plaid_Parser_hpp
#define Plaid_Parser_hpp

#include <boost/property_tree/ptree.hpp>
#include <string>
#include <vector>

#include "data/Account.hpp"
#include "data/Balance.hpp"
#include "data/Institution.hpp"
#include "data/Transaction.hpp"

namespace premia {
namespace plaid {

// Parse raw JSON responses from the Plaid API into typed model vectors/structs.
// Each function returns an empty container on parse failure.

std::vector<Account>     ParseAccounts(const std::string& json);
std::vector<Account>     ParseBalances(const std::string& json);
std::vector<Transaction> ParseTransactions(const std::string& json);
Institution              ParseInstitution(const std::string& json);

// Returns a flat list of category names from /categories/get
std::vector<std::string> ParseCategories(const std::string& json);

// Internal helper — exposed for testing.
Balance ParseBalance(const boost::property_tree::ptree& node);

}  // namespace plaid
}  // namespace premia

#endif  // Plaid_Parser_hpp
