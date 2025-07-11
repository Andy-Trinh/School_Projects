#ifndef A_LANG_DATA_TYPES
#define A_LANG_DATA_TYPES

#include <list>
#include <sstream>
#include "errors.hpp"

#include <unordered_map>

#ifndef A_LANG_MAP_ALIAS
// Use an alias template so that we can use
// "HashMap" and it means "std::unordered_map"
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;
#endif

namespace a_lang{

enum BaseType{
	INT, VOID, STR, BOOL
};

class DataType{
public:
    virtual std::string getString() const = 0;
};

class varType : public DataType {
public:

};
}

#endif
