#include "Halext.hpp"

using namespace halext;

Halext::Halext()=default;

bool & Halext::getPrivateBalance()
{
    return privateBalance;
}

void Halext::setPrivateBalance(bool private_balance)
{
    privateBalance = private_balance;
}

bool Halext::login(String username, String password) const
{
    return true;
}