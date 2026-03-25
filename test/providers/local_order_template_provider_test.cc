#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>

#include "premia/providers/local/order_template_provider.hpp"

namespace {

auto MakeTempDir(const std::string& name) -> std::filesystem::path {
  const auto dir = std::filesystem::temp_directory_path() / name;
  std::filesystem::remove_all(dir);
  std::filesystem::create_directories(dir);
  return dir;
}

void WriteFile(const std::filesystem::path& path, const std::string& content) {
  std::ofstream output(path);
  output << content;
}

auto MakeTemplate() -> premia::core::application::OrderTemplate {
  premia::core::application::OrderTemplate tmpl;
  tmpl.name = "Buy $500 VTI monthly";
  tmpl.symbol = "VTI";
  tmpl.order_type = "MARKET";
  tmpl.action = "BUY";
  tmpl.quantity = "500.00";
  tmpl.is_dollar_amount = true;
  tmpl.time_in_force = "DAY";
  tmpl.session = "NORMAL";
  tmpl.asset_type = "EQUITY";
  return tmpl;
}

}  // namespace

namespace premiatests::ProviderOrderTemplateTests {

TEST(LocalOrderTemplateProvider, ListReturnsEmptyWhenFileIsEmpty) {
  const auto dir = MakeTempDir("premia-template-list-empty");
  const auto path = dir / "order_templates.json";
  WriteFile(path, "{\"templates\": []}");
  premia::providers::local::OrderTemplateProvider provider(path.string());

  const auto templates = provider.ListTemplates();

  EXPECT_TRUE(templates.empty());
}

TEST(LocalOrderTemplateProvider, CreateAssignsIdAndTimestamps) {
  const auto dir = MakeTempDir("premia-template-create");
  const auto path = dir / "order_templates.json";
  WriteFile(path, "{\"templates\": []}");
  premia::providers::local::OrderTemplateProvider provider(path.string());

  const auto created = provider.CreateTemplate(MakeTemplate());

  EXPECT_FALSE(created.id.empty());
  EXPECT_EQ(created.name, "Buy $500 VTI monthly");
  EXPECT_EQ(created.symbol, "VTI");
  EXPECT_TRUE(created.is_dollar_amount);
  EXPECT_FALSE(created.created_at.empty());
  EXPECT_FALSE(created.updated_at.empty());
}

TEST(LocalOrderTemplateProvider, ListReturnsCreatedTemplate) {
  const auto dir = MakeTempDir("premia-template-list-after-create");
  const auto path = dir / "order_templates.json";
  WriteFile(path, "{\"templates\": []}");
  premia::providers::local::OrderTemplateProvider provider(path.string());

  provider.CreateTemplate(MakeTemplate());
  const auto templates = provider.ListTemplates();

  ASSERT_EQ(templates.size(), 1u);
  EXPECT_EQ(templates.front().name, "Buy $500 VTI monthly");
}

TEST(LocalOrderTemplateProvider, UpdateChangesFieldsAndPreservesCreatedAt) {
  const auto dir = MakeTempDir("premia-template-update");
  const auto path = dir / "order_templates.json";
  WriteFile(path, "{\"templates\": []}");
  premia::providers::local::OrderTemplateProvider provider(path.string());

  const auto created = provider.CreateTemplate(MakeTemplate());

  auto changed = MakeTemplate();
  changed.name = "Buy $1000 VTI";
  changed.quantity = "1000.00";
  const auto updated = provider.UpdateTemplate(created.id, changed);

  EXPECT_EQ(updated.id, created.id);
  EXPECT_EQ(updated.name, "Buy $1000 VTI");
  EXPECT_EQ(updated.quantity, "1000.00");
  EXPECT_EQ(updated.created_at, created.created_at);
}

TEST(LocalOrderTemplateProvider, UpdateThrowsForUnknownId) {
  const auto dir = MakeTempDir("premia-template-update-missing");
  const auto path = dir / "order_templates.json";
  WriteFile(path, "{\"templates\": []}");
  premia::providers::local::OrderTemplateProvider provider(path.string());

  EXPECT_THROW(provider.UpdateTemplate("nonexistent_id", MakeTemplate()),
               std::runtime_error);
}

TEST(LocalOrderTemplateProvider, DeleteRemovesTemplateFromList) {
  const auto dir = MakeTempDir("premia-template-delete");
  const auto path = dir / "order_templates.json";
  WriteFile(path, "{\"templates\": []}");
  premia::providers::local::OrderTemplateProvider provider(path.string());

  const auto created = provider.CreateTemplate(MakeTemplate());
  const auto deleted = provider.DeleteTemplate(created.id);

  EXPECT_EQ(deleted.id, created.id);
  EXPECT_TRUE(provider.ListTemplates().empty());
}

TEST(LocalOrderTemplateProvider, DeleteThrowsForUnknownId) {
  const auto dir = MakeTempDir("premia-template-delete-missing");
  const auto path = dir / "order_templates.json";
  WriteFile(path, "{\"templates\": []}");
  premia::providers::local::OrderTemplateProvider provider(path.string());

  EXPECT_THROW(provider.DeleteTemplate("nonexistent_id"), std::runtime_error);
}

TEST(LocalOrderTemplateProvider, MultipleTemplatesPersistedInOrder) {
  const auto dir = MakeTempDir("premia-template-multiple");
  const auto path = dir / "order_templates.json";
  WriteFile(path, "{\"templates\": []}");
  premia::providers::local::OrderTemplateProvider provider(path.string());

  auto t1 = MakeTemplate();
  t1.name = "First";
  auto t2 = MakeTemplate();
  t2.name = "Second";

  provider.CreateTemplate(t1);
  provider.CreateTemplate(t2);
  const auto templates = provider.ListTemplates();

  ASSERT_EQ(templates.size(), 2u);
  EXPECT_EQ(templates[0].name, "First");
  EXPECT_EQ(templates[1].name, "Second");
}

}  // namespace premiatests::ProviderOrderTemplateTests
