#ifndef EventInterface_hpp
#define EventInterface_hpp

#include "core.hpp"
#include <typeindex>
#include <typeinfo>
#include <cassert>

namespace Premia {

    struct EventInterface 
    {
        std::unordered_map<std::string, std::pair<VoidEventHandler, std::type_index>> events;

        template<typename T>
        void insert(std::string eventName, T event) 
        {
            auto eventType = std::type_index(typeid(event));                     
            events.insert(std::make_pair(eventName, 
                        std::make_pair((VoidEventHandler) event, eventType))); 
        }

        template<typename T, typename... Args>
        T trigger(std::string eventName, Args&&... args) 
        {
            auto eventIterator = events.find(eventName);                            
            auto eventValue = eventIterator->second;                                  
            auto typeCastedEvent = (T(*)(Args ...))(eventValue.first);              
            assert(eventValue.second == std::type_index(typeid(typeCastedEvent)));  
            return typeCastedEvent(std::forward<Args>(args)...);                  
        }

        
    };

    template <> void EventInterface::insert(std::string, TDALoginEvent); 
    template <> void EventInterface::trigger(std::string, const std::string & key, const std::string & token); 
}

#endif