#ifndef Halext_hpp
#define Halext_hpp

#include "Premia.hpp"
#include "Client.hpp"
#include "User.hpp"

namespace halext 
{
    class Halext
    {
    private:
        Client client;
        User currentUser;
        bool privateBalance = false;

    public:
        Halext();

        bool & getPrivateBalance();
        void setPrivateBalance(bool private_balance);

        bool login(String username, String password) const;

    };
}

#endif