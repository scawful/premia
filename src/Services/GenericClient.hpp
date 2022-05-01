#ifndef GenericClient_hpp
#define GenericClient_hpp

#include "Premia.hpp"
#include "Curl.hpp"
#include <iostream>

class GenericClient
{
private:
    static size_t json_write_callback(const char * contents, size_t size, size_t nmemb, std::string *s);

    std::string send_request(String endpoint) const;

public:
    GenericClient();

    std::string get_spx_gex() const;

};

#endif