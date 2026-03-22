#include "parser.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <sstream>

namespace premia {
namespace plaid {

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

Balance ParseBalance(const boost::property_tree::ptree& node) {
  Balance b;
  b.current             = node.get<double>("current", 0.0);
  b.available           = node.get<double>("available", 0.0);
  b.limit               = node.get<double>("limit", 0.0);
  b.iso_currency_code   = node.get<std::string>("iso_currency_code", "");
  return b;
}

static Account ParseAccountNode(const boost::property_tree::ptree& node) {
  Account a;
  a.account_id    = node.get<std::string>("account_id", "");
  a.name          = node.get<std::string>("name", "");
  a.official_name = node.get<std::string>("official_name", "");
  a.type          = node.get<std::string>("type", "");
  a.subtype       = node.get<std::string>("subtype", "");
  a.mask          = node.get<std::string>("mask", "");

  auto balances_node = node.get_child_optional("balances");
  if (balances_node) {
    a.balances = ParseBalance(*balances_node);
  }
  return a;
}

// ---------------------------------------------------------------------------
// Public parse functions
// ---------------------------------------------------------------------------

std::vector<Account> ParseAccounts(const std::string& json) {
  std::vector<Account> result;
  if (json.empty()) return result;

  try {
    std::istringstream ss(json);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    auto accounts_node = pt.get_child_optional("accounts");
    if (!accounts_node) return result;

    for (const auto& [key, node] : *accounts_node) {
      result.push_back(ParseAccountNode(node));
    }
  } catch (const std::exception& e) {
    std::cerr << "[Plaid] ParseAccounts error: " << e.what() << "\n";
  }
  return result;
}

// /accounts/balance/get returns the same accounts array with fresh balances.
std::vector<Account> ParseBalances(const std::string& json) {
  return ParseAccounts(json);
}

std::vector<Transaction> ParseTransactions(const std::string& json) {
  std::vector<Transaction> result;
  if (json.empty()) return result;

  try {
    std::istringstream ss(json);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    auto txns_node = pt.get_child_optional("transactions");
    if (!txns_node) return result;

    for (const auto& [key, node] : *txns_node) {
      Transaction t;
      t.transaction_id    = node.get<std::string>("transaction_id", "");
      t.account_id        = node.get<std::string>("account_id", "");
      t.amount            = node.get<double>("amount", 0.0);
      t.date              = node.get<std::string>("date", "");
      t.name              = node.get<std::string>("name", "");
      t.merchant_name     = node.get<std::string>("merchant_name", "");
      t.category_id       = node.get<std::string>("category_id", "");
      t.iso_currency_code = node.get<std::string>("iso_currency_code", "");
      t.pending           = node.get<bool>("pending", false);

      auto cat_node = node.get_child_optional("category");
      if (cat_node) {
        for (const auto& [ck, cv] : *cat_node) {
          t.category.push_back(cv.get_value<std::string>());
        }
      }

      result.push_back(std::move(t));
    }
  } catch (const std::exception& e) {
    std::cerr << "[Plaid] ParseTransactions error: " << e.what() << "\n";
  }
  return result;
}

Institution ParseInstitution(const std::string& json) {
  Institution inst;
  if (json.empty()) return inst;

  try {
    std::istringstream ss(json);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    auto inst_node = pt.get_child_optional("institution");
    if (!inst_node) return inst;

    inst.institution_id = inst_node->get<std::string>("institution_id", "");
    inst.name           = inst_node->get<std::string>("name", "");
    inst.logo           = inst_node->get<std::string>("logo", "");
    inst.primary_color  = inst_node->get<std::string>("primary_color", "");
    inst.url            = inst_node->get<std::string>("url", "");

    auto products_node = inst_node->get_child_optional("products");
    if (products_node) {
      for (const auto& [pk, pv] : *products_node) {
        inst.products.push_back(pv.get_value<std::string>());
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "[Plaid] ParseInstitution error: " << e.what() << "\n";
  }
  return inst;
}

std::vector<std::string> ParseCategories(const std::string& json) {
  std::vector<std::string> result;
  if (json.empty()) return result;

  try {
    std::istringstream ss(json);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);

    auto cats_node = pt.get_child_optional("categories");
    if (!cats_node) return result;

    for (const auto& [key, cat_node] : *cats_node) {
      // Use the category_id + hierarchy string as a descriptive entry.
      std::string category_id = cat_node.get<std::string>("category_id", "");

      auto hier_node = cat_node.get_child_optional("hierarchy");
      std::string full_name;
      if (hier_node) {
        for (const auto& [hk, hv] : *hier_node) {
          if (!full_name.empty()) full_name += " > ";
          full_name += hv.get_value<std::string>();
        }
      }

      if (!full_name.empty()) {
        result.push_back(full_name);
      } else if (!category_id.empty()) {
        result.push_back(category_id);
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "[Plaid] ParseCategories error: " << e.what() << "\n";
  }
  return result;
}

}  // namespace plaid
}  // namespace premia
