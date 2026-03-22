#ifndef PREMIA_API_HTTP_JSON_H
#define PREMIA_API_HTTP_JSON_H

#include <string>
#include <vector>

#include "premia/core/application/screen_models.hpp"
#include "premia/core/application/workflow_models.hpp"

namespace premia::api::http {

auto SerializeHealthResponse() -> std::string;
auto SerializeBootstrapResponse(const core::application::BootstrapData& data)
    -> std::string;
auto SerializeHomeScreenResponse(const core::application::HomeScreenData& data)
    -> std::string;
auto SerializeWatchlistsResponse(
    const std::vector<core::application::WatchlistSummary>& watchlists)
    -> std::string;
auto SerializeWatchlistScreenResponse(
    const core::application::WatchlistScreenData& data) -> std::string;
auto SerializeWatchlistResponse(const core::application::WatchlistSummary& data)
    -> std::string;
auto SerializeQuoteScreenResponse(const core::application::QuoteDetail& data)
    -> std::string;
auto SerializeChartScreenResponse(const core::application::ChartScreenData& data)
    -> std::string;
auto SerializeConnectionSummaryResponse(
    const core::application::ConnectionSummary& data) -> std::string;
auto SerializeSchwabOAuthStartResponse(
    const core::application::SchwabOAuthStartData& data) -> std::string;
auto SerializePlaidLinkTokenResponse(
    const core::application::PlaidLinkTokenData& data) -> std::string;
auto SerializeErrorResponse(const std::string& code, const std::string& message,
                            const std::string& action,
                            const std::string& provider = "") -> std::string;

}  // namespace premia::api::http

#endif  // PREMIA_API_HTTP_JSON_H
