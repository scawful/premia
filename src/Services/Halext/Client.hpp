#ifndef HalextClient_hpp
#define HalextClient_hpp

#include "core.hpp"

namespace halext 
{
    class Client 
    {
    private:
        void establish_connection();
        
    public:
        Client();
    };
}

#endif