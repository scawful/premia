#ifndef GenericClient_hpp
#define GenericClient_hpp

#include "Premia.hpp"
#include "Library/Curl.hpp"
#include <iostream>

class GenericClient
{
private:
    static size_t json_write_callback(const char * contents, size_t size, size_t nmemb, String *s);

    String send_request(CRString endpoint) const;

public:
    GenericClient();

    String get_spx_gex() const;

};

#endif