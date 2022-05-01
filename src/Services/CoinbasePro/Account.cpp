#include "Account.hpp"

namespace cbp
{
    /**
     * @brief Get an account positions 
     * 
     * @param ticker 
     * @return std::unordered_map<std::string, std::string> 
     */
    std::unordered_map<std::string, std::string> Account::get_position(const std::string & ticker)
    {
        for (const auto & fields: accounts_vector) {
            for (const auto & [key, value]: fields) {
                if (value == ticker) {
                    return fields;
                }
            }
        }
        return accounts_vector[0];
    }

}