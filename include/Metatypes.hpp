#ifndef Metatype_hpp
#define Metatype_hpp
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <iostream>
#include <cmath>
#include <memory>
#include <string>
#include <vector>
#include <tuple>
#include <mutex>
#include <exception>
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
using Logger               = MetaFunc<String>;
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
    E trigger(CRString key, Args&&... args) {
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

    // called iff f successful
    std::call_once(once, std::bind(std::move(t), std::forward<Args>(args)...));
}

template <typename Lambda, typename Theta>
inline auto try_catch_finally(const Lambda &f, const Theta &finally) 
    noexcept(false)
    -> void {
    try {
        f();
    } catch (...) {
        try {
            finally();
        } catch (...) { 
            std::terminate();
        }
        throw;
    }
    finally();
}

class never_thrown_exception {};

#define Try          try_catch_finally([&](){ try
#define finally      catch(never_thrown_exception){throw;} },[&]()
#define Proceed      ) 

class Destruction 
    : public std::exception {};

template <typename C, typename I, typename D>
inline auto cons_ins_des(const C &c, const I &i, const D &d)
    noexcept(false)
    -> void {
    static once_flag once;
    try {
        std::call_once(once, [c] () { 
            c(); 
        });
        
        i();
    } catch (const Destruction & e) {
        d();
    }
}

#define Construct   cons_ins_des([&]()
#define Instruct    ,[&]()
#define Destruct    ,[&]()
#define RecursiveDestruct   ,[&]() { throw Destruction(); })

#define canDestroy  catch (const Premia::FatalException & e) { \
                        std::cout << e.what() << std::endl; \
                        throw Destruction(); \
                    } \

#endif