#ifndef Metatype_hpp
#define Metatype_hpp
#define _USE_MATH_DEFINES

#include <stdio.h>

#include <cassert>
#include <cmath>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <tuple>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>


template <typename... Args>
using MetaFunc = std::function<void(Args &&...)>;

using EventHandler = MetaFunc<>;
using Logger = MetaFunc<std::string>;
using CRLogger = const Logger &;
using SocketListener = MetaFunc<const char *>;

using TypeIndex = std::type_index;

template <class... Types>
using Tuple = std::tuple<Types...>;

template <class K, class V>
using AbstractMap = std::unordered_map<K, V>;

using EventMap = AbstractMap<std::string, EventHandler>;
using AbstractEventMap =
    AbstractMap<std::string, std::pair<EventHandler, TypeIndex>>;

struct Events {
  AbstractEventMap events;

  template <typename E>
  void insert(std::string key, E event) {
    auto eventType = TypeIndex(typeid(event));
    events.insert(
        std::make_pair(key, std::make_pair((EventHandler)event, eventType)));
  }

  template <typename E, typename... Args>
  E trigger(const std::string &key, Args &&...args) {
    auto eventIt = events.find(key);
    auto eventValue = eventIt->second;
    auto typeCastedEvent = (E(*)(Args...))(eventValue.first);
    assert(eventValue.second == TypeIndex(typeid(typeCastedEvent)));
    return typeCastedEvent(std::forward<Args>(args)...);
  }
};

using std::once_flag;

template <typename Lambda, typename Theta>
void runOnce(Lambda f, Theta t) {
  static once_flag once;

  f();

  std::call_once(once, [t]() { t(); });
}

template <typename Lambda, typename Theta, typename... Args>
void runOnceArgs(Lambda f, Theta t, Args &&...args) {
  static once_flag once;

  f();  // may return

  // called iff f successful
  std::call_once(once, std::bind(std::move(t), std::forward<Args>(args)...));
}

template <typename Lambda, typename Theta>
inline auto try_catch_finally(const Lambda &f,
                              const Theta &finally) noexcept(false) -> void {
  try {
    f();
  } catch (...) {
    try {
      finally();
    } catch (...) {
      std::terminate();
    }
    throw;
  } finally();
}

class never_thrown_exception {};

#define Try          try_catch_finally([&](){ try
#define finally                    \
  catch (never_thrown_exception) { \
    throw;                         \
  }                                \
  }                                \
  , [&]()
#define PROCEED      )

class Destruction : public std::exception {};

template <typename C, typename I, typename D>
inline auto cons_ins_des(const C &c, const I &i, const D &d) noexcept(false)
    -> void {
  static once_flag once;
  try {
    std::call_once(once, [c]() { c(); });

    i();
  } catch (const Destruction &e) {
    d();
  }
}

#define CONSTRUCT   cons_ins_des([&]()
#define INSTRUCT , [&]()
#define DESTRUCT , [&]()
#define RecursiveDestruct   ,[&]() { throw Destruction(); })

#define canDestroy                          \
  catch (const premia::FatalException &e) { \
    std::cout << e.what() << std::endl;     \
    throw Destruction();                    \
  }

#endif