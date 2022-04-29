#include "WatchlistModel.hpp"

void WatchlistModel::initWatchlist()
{
    std::string account_num = getTDAInterface().get_all_accounts().at(0);
    watchlists = getTDAInterface().getWatchlistsByAccount(account_num);

    for ( int i = 0; i < watchlists.size(); i++ ) {
        watchlistNames.push_back(watchlists[i].getName());
        openList.push_back(0);
    }

    for (std::string const& str : watchlistNames) {
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

std::string WatchlistModel::getWatchlistName(int index)
{
    return watchlistNames.at(index);
}

tda::Quote & WatchlistModel::getQuote(std::string key)
{
    return quotes.at(key);
}

tda::Watchlist & WatchlistModel::getWatchlist(int index)
{
    return watchlists.at(index);
}

void WatchlistModel::setQuote(std::string key, tda::Quote quote)
{
    this->quotes[key] = quote;
}

std::vector<const char *> WatchlistModel::getWatchlistNamesCharVec()
{
    return watchlistNamesChar;
}