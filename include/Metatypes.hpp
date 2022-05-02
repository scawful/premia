#ifndef Metatype_hpp
#define Metatype_hpp

#include <string>
#include <vector>
#include <tuple>
#include <mutex>
#include <typeindex>
#include <typeinfo>
#include <cassert>
#include <unordered_map>
#include <functional>

using String               = std::string;
using RString              = String &;
using CString              = const String;
using CRString             = CString &;

template<typename... Args>
    using MetaFunc         = std::function<void(Args&&...)>;

using EventHandler         = MetaFunc<>;
using ConsoleLogger        = MetaFunc<String>;
using SocketListener       = MetaFunc<const char*>;

using TypeIndex            = std::type_index;

template <class E>
    using ArrayList        = std::vector<E>;
template <class E>
    using ListList         = ArrayList<ArrayList<E>>;

using IntList              = ArrayList<int>;
using DoubleList           = ArrayList<double>;
using StringList           = ArrayList<String>;

template <class... Types>
    using Tuple            = std::tuple<Types ...>;
    
template <class K, class V>
    using AbstractMap      = std::unordered_map<K, V>; 

using StringMap            = AbstractMap<String, String>;
using EventMap             = AbstractMap<String, EventHandler>;
using AbstractEventMap     = AbstractMap<String, std::pair<EventHandler, TypeIndex>>;

struct Events {
    AbstractEventMap events;

    template <typename E>
    void insert(String key, E event) {
        auto eventType = TypeIndex(typeid(event));                     
        events.insert(std::make_pair(key, 
                      std::make_pair((EventHandler) event, eventType))); 
    }

    template <typename E, typename... Args>
    E trigger(String key, Args&&... args) {
        auto eventIt = events.find(key);                            
        auto eventValue = eventIt->second;                                  
        auto typeCastedEvent = (E (*) (Args ...)) (eventValue.first);              
        assert(eventValue.second == TypeIndex(typeid(typeCastedEvent)));  
        return typeCastedEvent(std::forward<Args>(args)...);                  
    }

};

using std::once_flag;

template <typename Lambda, typename Theta>
void runOnce(Lambda f, Theta t){
    static once_flag once; 
    
    f();

    std::call_once(once, [t] () {
        t();
    });
}

template <typename Lambda, typename Theta, typename... Args>
void runOnceArgs(Lambda f, Theta t, Args&&... args){
    static once_flag once; 

    f(); // may return 

    auto pragma = [t] (Args&&... args) {
        std::bind(std::move(t), 
                  std::forward<Args>(args)...);
    }; 

    std::call_once(once, pragma); // only called if rest of function successful
}

#endif