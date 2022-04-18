#include "Halext.hpp"

using namespace halext;

Halext::Halext() : privateBalance(false)
{
}

bool & Halext::getPrivateBalance()
{
    return privateBalance;
}

void Halext::setPrivateBalance(bool private_balance)
{
    privateBalance = private_balance;
}