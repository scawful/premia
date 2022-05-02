#include "WatchlistModel.hpp"

void WatchlistModel::initWatchlist()
{
    String account_num;
    try {
        account_num = getTDAInterface().get_all_accounts().at(0);
    } catch (const std::out_of_range & e) {
        String error(e.what());
        logger("[error] " + error);
    }
    watchlists = getTDAInterface().getWatchlistsByAccount(account_num);

    for ( int i = 0; i < watchlists.size(); i++ ) {
        watchlistNames.push_back(watchlists[i].getName());
        openList.push_back(0);
    }

    for (String const& str : watchlistNames) {
        watchlistNamesChar.push_back(str.data());
    }

    active = true;
}

bool WatchlistModel::getOpenList(int n)
{
    return openList.at(n);
}

void WatchlistModel::setOpenList(int n)
{
    this->openList[n] = 1;
}

bool WatchlistModel::isActive() const
{
    return active;
}

void WatchlistModel::addLogger(const Premia::ConsoleLogger & newLogger)
{
    this->logger = newLogger;
}

String WatchlistModel::getWatchlistName(int index)
{
    return watchlistNames.at(index);
}

tda::Quote & WatchlistModel::getQuote(CRString key)
{
    return quotes.at(key);
}

tda::Watchlist & WatchlistModel::getWatchlist(int index)
{
    return watchlists.at(index);
}

void WatchlistModel::setQuote(CRString key, const tda::Quote & quote)
{
    this->quotes[key] = quote;
}

std::vector<const char *> WatchlistModel::getWatchlistNamesCharVec() const
{
    return watchlistNamesChar;
}