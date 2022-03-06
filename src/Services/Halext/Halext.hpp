#ifndef Halext_hpp
#define Halext_hpp

#include "../../core.hpp"
#include "Client.hpp"
#include "User.hpp"

namespace halext 
{
    class Halext
    {
    private:
        Client client;
        User currentUser;

    public:
        Halext();

        bool privateBalance;
    };
}

#endif