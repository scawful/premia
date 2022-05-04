#ifndef HalextClient_hpp
#define HalextClient_hpp

#include "Premia.hpp"
#include "Library/Curl.hpp"
#include "Metatypes.hpp"

namespace halext 
{
    class Client 
    {
    private:
        String username;
        
    public:
        Client()=default;

        String send_request(CRString endpoint) const;
    };
}

#endif