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
auto SerializeMultiAccountHomeScreenResponse(
    const core::application::MultiAccountHomeScreen& data) -> std::string;
auto SerializeAccountScreenResponse(const core::application::AccountDetail& data)
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
auto SerializeOptionChainResponse(const core::application::OptionChainSnapshot& data)
    -> std::string;
auto SerializeOrderPreviewResponse(const core::application::OrderPreviewData& data)
    -> std::string;
auto SerializeOrderSubmissionResponse(
    const core::application::OrderSubmissionData& data) -> std::string;
auto SerializeOrderCancellationResponse(
    const core::application::OrderCancellationData& data) -> std::string;
auto SerializeOrderReplacementResponse(
    const core::application::OrderReplacementData& data) -> std::string;
auto SerializeOrderRecordsResponse(
    const std::vector<core::application::OrderRecordData>& data) -> std::string;
auto SerializeOrderTemplateResponse(
    const core::application::OrderTemplate& data) -> std::string;
auto SerializeOrderTemplatesResponse(
    const std::vector<core::application::OrderTemplate>& data) -> std::string;
auto SerializeConnectionSummaryResponse(
    const core::application::ConnectionSummary& data) -> std::string;
auto SerializeSchwabOAuthStartResponse(
    const core::application::SchwabOAuthStartData& data) -> std::string;
auto SerializePlaidLinkTokenResponse(
    const core::application::PlaidLinkTokenData& data) -> std::string;
auto SerializeRSUOverlayResponse(const core::application::RSUOverlayScreen& data)
    -> std::string;
auto SerializeErrorResponse(const std::string& code, const std::string& message,
                            const std::string& action,
                            const std::string& provider = "") -> std::string;

}  // namespace premia::api::http

#endif  // PREMIA_API_HTTP_JSON_H
