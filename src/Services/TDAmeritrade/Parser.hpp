#ifndef Parser_hpp
#define Parser_hpp

#include "Watchlist.hpp"

namespace tda 
{

    class Parser 
    {
    private:

    public:
        Parser();

        std::vector<Watchlist> parse_watchlist_data( boost::property_tree::ptree data );        
    };

}

#endif