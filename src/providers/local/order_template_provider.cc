#include "premia/providers/local/order_template_provider.hpp"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace premia::providers::local {

namespace application = premia::core::application;
namespace pt = boost::property_tree;

namespace {

auto MakeRuntimeId(const std::string& prefix) -> std::string {
  const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
  return prefix + "_" + std::to_string(ticks);
}

auto ParseTemplate(const pt::ptree& node) -> application::OrderTemplate {
  application::OrderTemplate tmpl;
  tmpl.id = node.get<std::string>("id", "");
  tmpl.name = node.get<std::string>("name", "");
  tmpl.symbol = node.get<std::string>("symbol", "");
  tmpl.order_type = node.get<std::string>("orderType", "MARKET");
  tmpl.action = node.get<std::string>("action", "BUY");
  tmpl.quantity = node.get<std::string>("quantity", "0");
  tmpl.is_dollar_amount = node.get<bool>("isDollarAmount", false);
  tmpl.time_in_force = node.get<std::string>("timeInForce", "DAY");
  tmpl.session = node.get<std::string>("session", "NORMAL");
  tmpl.asset_type = node.get<std::string>("assetType", "EQUITY");
  tmpl.provider_preference = node.get<std::string>("providerPreference", "");
  tmpl.created_at = node.get<std::string>("createdAt", "");
  tmpl.updated_at = node.get<std::string>("updatedAt", "");
  return tmpl;
}

auto SerializeTemplate(const application::OrderTemplate& tmpl) -> pt::ptree {
  pt::ptree node;
  node.put("id", tmpl.id);
  node.put("name", tmpl.name);
  node.put("symbol", tmpl.symbol);
  node.put("orderType", tmpl.order_type);
  node.put("action", tmpl.action);
  node.put("quantity", tmpl.quantity);
  node.put("isDollarAmount", tmpl.is_dollar_amount);
  node.put("timeInForce", tmpl.time_in_force);
  node.put("session", tmpl.session);
  node.put("assetType", tmpl.asset_type);
  node.put("providerPreference", tmpl.provider_preference);
  node.put("createdAt", tmpl.created_at);
  node.put("updatedAt", tmpl.updated_at);
  return node;
}

auto ReadRoot(const std::string& path) -> pt::ptree {
  pt::ptree root;
  std::ifstream input(path);
  if (input.good()) {
    try {
      pt::read_json(input, root);
    } catch (const std::exception&) {
      root = pt::ptree{};
    }
  }
  return root;
}

auto WriteRoot(const std::string& path, const pt::ptree& root) -> void {
  std::ofstream output(path);
  pt::write_json(output, root);
}

}  // namespace

OrderTemplateProvider::OrderTemplateProvider(std::string path)
    : path_(std::move(path)) {}

auto OrderTemplateProvider::ListTemplates() const
    -> std::vector<application::OrderTemplate> {
  const auto root = ReadRoot(path_);
  std::vector<application::OrderTemplate> templates;
  if (auto child = root.get_child_optional("templates")) {
    for (const auto& item : child.get()) {
      templates.push_back(ParseTemplate(item.second));
    }
  }
  return templates;
}

auto OrderTemplateProvider::CreateTemplate(
    const application::OrderTemplate& tmpl)
    -> application::OrderTemplate {
  auto root = ReadRoot(path_);

  pt::ptree templates;
  if (auto child = root.get_child_optional("templates")) {
    templates = child.get();
  }

  application::OrderTemplate created = tmpl;
  created.id = MakeRuntimeId("tmpl");
  const auto now = CurrentUtcTimestamp();
  created.created_at = now;
  created.updated_at = now;

  templates.push_back({"", SerializeTemplate(created)});
  root.put_child("templates", templates);
  WriteRoot(path_, root);

  return created;
}

auto OrderTemplateProvider::UpdateTemplate(
    const std::string& id, const application::OrderTemplate& tmpl)
    -> application::OrderTemplate {
  auto root = ReadRoot(path_);

  pt::ptree templates;
  if (auto child = root.get_child_optional("templates")) {
    templates = child.get();
  }

  bool found = false;
  application::OrderTemplate updated;
  for (auto& item : templates) {
    if (item.second.get<std::string>("id", "") == id) {
      updated = tmpl;
      updated.id = id;
      updated.created_at = item.second.get<std::string>("createdAt", "");
      updated.updated_at = CurrentUtcTimestamp();
      item.second = SerializeTemplate(updated);
      found = true;
      break;
    }
  }
  if (!found) {
    throw std::runtime_error("order template not found: " + id);
  }

  root.put_child("templates", templates);
  WriteRoot(path_, root);
  return updated;
}

auto OrderTemplateProvider::DeleteTemplate(const std::string& id)
    -> application::OrderTemplate {
  auto root = ReadRoot(path_);

  pt::ptree templates;
  if (auto child = root.get_child_optional("templates")) {
    templates = child.get();
  }

  application::OrderTemplate deleted;
  bool found = false;
  pt::ptree remaining;
  for (const auto& item : templates) {
    if (item.second.get<std::string>("id", "") == id) {
      deleted = ParseTemplate(item.second);
      found = true;
    } else {
      remaining.push_back({"", item.second});
    }
  }
  if (!found) {
    throw std::runtime_error("order template not found: " + id);
  }

  root.put_child("templates", remaining);
  WriteRoot(path_, root);
  return deleted;
}

auto OrderTemplateProvider::CurrentUtcTimestamp() const -> std::string {
  const auto now = std::chrono::system_clock::now();
  const std::time_t now_time = std::chrono::system_clock::to_time_t(now);
  std::tm utc_time{};
#if defined(_WIN32)
  gmtime_s(&utc_time, &now_time);
#else
  gmtime_r(&now_time, &utc_time);
#endif
  std::ostringstream oss;
  oss << std::put_time(&utc_time, "%Y-%m-%dT%H:%M:%SZ");
  return oss.str();
}

}  // namespace premia::providers::local
