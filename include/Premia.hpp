#ifndef Premia_hpp
#define Premia_hpp

#include "Metatypes.hpp"

namespace Premia {

    class ClientException : public std::exception {};
    
    class TDAClientException 
        : public ClientException {
        private:
            const char * msg_src;
            const char * info_ex;

        public:
            TDAClientException(const char * msg, const char * info) 
                : msg_src(msg), info_ex(info) { }

            const char * what() const noexcept override { return msg_src; }
            const char * info() const { return info_ex; }
    };

    

}

#endif