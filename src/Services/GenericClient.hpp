#ifndef GenericClient_hpp
#define GenericClient_hpp

#include "../core.hpp"

class GenericClient
{
private:
        static size_t json_write_callback(void *contents, size_t size, size_t nmemb, std::string *s);

    std::string send_request(std::string endpoint);

public:
    GenericClient();

    std::string get_spx_gex();

};

#endif