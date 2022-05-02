#ifndef Metatype_hpp
#define Metatype_hpp

#include <string>
#include <vector>
#include <unordered_map>

using String     = std::string;
using RString    = std::string &;
using CString    = const std::string;
using CRString   = const std::string &;

using IntList    = std::vector<int>;
using DoubleList = std::vector<double>;
using StringList = std::vector<String>;

using StringMap  = std::unordered_map<String, String>;

template<class K, class V>
    using AbstractMap = std::unordered_map<K, V>; 

#endif