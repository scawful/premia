#ifndef HalextClient_hpp
#define HalextClient_hpp

#include "Premia.hpp"

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