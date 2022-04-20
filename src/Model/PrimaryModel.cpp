#include "PrimaryModel.hpp"

void PrimaryModel::login(const std::string & username, const std::string & password)
{
    if ( HalextInterface.login(username, password) ) {
        callbacks["login"]();
    }
}
