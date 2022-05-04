#ifndef CurlInclude_hpp
#define CurlInclude_hpp

#include <curl/curl.h>
#include <openssl/ssl.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

namespace curlbacks
{
    /**
     * @brief Get JSON data from server and store into const ref String 
     * @author @scawful
     * 
     * @param contents 
     * @param size 
     * @param nmemb 
     * @param s 
     * @return size_t 
     */
    static size_t json_write(const char * contents, size_t size, size_t nmemb, String *s)
    {
        size_t new_length = size * nmemb;
        try {
            s->append(contents, new_length);
        } catch(const std::bad_alloc &e) {
            // @todo attach a logger 
            return EXIT_FAILURE;
        }
        return new_length;
    }
} // namespace curlbacks (lol)


#endif