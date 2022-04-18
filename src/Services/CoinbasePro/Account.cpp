#include "Account.hpp"

namespace cbp
{
    /**
     * @brief Initialize the property tree variables into Account map
     * @todo needs proper parsing 
     * 
     */
    void Account::initVariables()
    {
        for (const auto & [key, value] : accountData) {
            std::unordered_map< std::string, std::string > new_position_data;
            for (const auto & [dataKey, dataValue]: value) {
                new_position_data[dataKey] = dataValue.get_value<std::string>();
            }
            accounts_vector.push_back( new_position_data );
        }
    }

    Account::Account(const boost::property_tree::ptree & account_data) : accountData(account_data) {
        initVariables();
    }

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