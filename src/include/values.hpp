#ifndef VALUES_JINJA
#define VALUES_JINJA

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace Jinja {
// these are the types that i will be accepting
enum class Types {
  INT,
  DOUBLE,
  BOOL,
  OBJECT,
  STRING,
  LIST,
  NIL,
  UNKOWN,

};
struct Value;
// where i am storing the information
using AcceptableKeyTypes = std::variant<int, double, bool, std::string>;
using ObjectInitializer =
    std::initializer_list<std::pair<AcceptableKeyTypes, Value>>;
using ObjectType = std::map<AcceptableKeyTypes, Value>;

using ListInitializer = std::initializer_list<Value>;
using ListType = std::vector<Value>;

// just a util that i will be using for various stuff
// a typesafe union that i will be using for keeping track of my values
using TypeValue = std::variant<int, double, bool, std::string,
                               ObjectType, ListType, std::nullptr_t>;

// with this we will be storing the variables that are going to be defined in an
// outside map.

struct Value {
  // this is just a small util
  //
  TypeValue variable = nullptr;
  Types type = Types::UNKOWN;
  Value() = default;

  // Constructor template para todos los tipos básicos
  template <typename T>
  Value(T &&v,
        std::enable_if_t<std::is_constructible_v<TypeValue, T>> * = nullptr)
      : variable(std::forward<T>(v)) {
    using DecayT = std::decay_t<T>;
    if constexpr (std::is_same_v<DecayT, int>)
      type = Types::INT;
    else if constexpr (std::is_same_v<DecayT, double>)
      type = Types::DOUBLE;
    else if constexpr (std::is_same_v<DecayT, bool>)
      type = Types::BOOL;
    else if constexpr (std::is_same_v<DecayT, std::string> ||
                       std::is_same_v<DecayT, const char *>)
      type = Types::STRING;
    else if constexpr (std::is_same_v<DecayT, ObjectType>)
      type = Types::OBJECT;
    else if constexpr (std::is_same_v<DecayT, ListType>)
      type = Types::LIST;
    else if constexpr (std::is_same_v<DecayT, std::nullptr_t>)
      type = Types::NIL;
  }
  Value(const char *v) : variable(std::string(v)), type(Types::STRING) {}
  Value(const Value &v) : variable(v.variable), type(v.type) {}
  template <typename T>
  std::enable_if_t<std::is_constructible_v<TypeValue, T>, Value &>
  operator=(T &&v) {
    using DecayT = std::decay_t<T>;
    variable = std::forward<T>(v);

    if constexpr (std::is_same_v<DecayT, int>)
      type = Types::INT;
    
    else if constexpr (std::is_same_v<DecayT, double>)
      type = Types::DOUBLE;
    else if constexpr (std::is_same_v<DecayT, bool>)
      type = Types::BOOL;
    else if constexpr (std::is_same_v<DecayT, std::string>)
      type = Types::STRING;
    else if constexpr (std::is_same_v<DecayT, ObjectType>)
      type = Types::OBJECT;
    else if constexpr (std::is_same_v<DecayT, ListType>)
      type = Types::LIST;
    else if constexpr (std::is_same_v<DecayT, std::nullptr_t>)
      type = Types::NIL;

    return *this;
  }
  Value &operator=(const Value v) {
    variable = v.variable;
    type = v.type;
    return *this;
  }

  inline void print(int indent = 0, bool padding = true,
                    std::string endline = "\n") {
    std::string pad;
    for (int i = 0; i < indent && padding; i++) {
      pad += " ";
    }

    switch (type) {
    case Types::INT:
      std::cout << pad << "[INT]" << std::get<int>(variable) << endline;
      break;

    case Types::DOUBLE:
      std::cout << pad << "[DOUBLE]" << std::get<double>(variable) << endline;
      break;
    case Types::STRING:
      std::cout << pad << "[STRING] \"" << std::get<std::string>(variable)
                << "\"" << endline;
      break;
    case Types::UNKOWN:
      std::cout << pad << "[UNKOWN]" << endline;
      break;
    case Types::NIL:
      std::cout << pad << "[NIL]" << endline;
      break;

    case Types::BOOL:
      std::cout << pad << "[BOOL]"
                << (std::get<bool>(variable) ? "true" : "false") << endline;
      break;
    // Just printing
    case Types::OBJECT: {
      ObjectType o = std::get<ObjectType>(variable);
      std::cout << pad << "{" << endline;
      for (auto &[k, v] : o) {
        std::cout << pad << "    ";
        std::visit(
            [&k](auto &&arg) {
              using DecayT = std::decay_t<decltype(arg)>;
              if constexpr (std::is_same_v<DecayT, int>)
                std::cout << std::get<int>(k);
              else if constexpr (std::is_same_v<DecayT, double>)
                std::cout << std::get<double>(k);
              else if constexpr (std::is_same_v<DecayT, std::string>)
                std::cout << std::get<std::string>(k);
              else if constexpr (std::is_same_v<DecayT, bool>)
                std::cout << (std::get<bool>(k) ? "True" : "False");
            },
            k);
        std::cout << ":" << endline;
        v.print(indent + 4);
      }
      std::cout << pad << "}" << endline;
    } break;
    // Printing
    case Types::LIST: {

      std::cout << pad << "[\n";
      ListType l = std::get<ListType>(variable);
      for (size_t i = 0; i < l.size(); ++i) {
        Value v = l[i];
        v.print(indent + 2, true, i != (l.size() - 1) ? ",\n" : "\n");
      }
      std::cout << pad << "]" << endline;
    } break;
    }
  }
  inline std::string to_string() {
    switch (type) {
    case Types::INT:
      return std::to_string(std::get<int>(variable));

    case Types::DOUBLE:
      return std::to_string(std::get<double>(variable));
    case Types::STRING:
      return std::get<std::string>(variable);
    case Types::BOOL:
      return std::get<bool>(variable) ? "True" : "False";
    case Types::NIL:
      return "NIL";
    case Types::LIST: {
      ListType list = std::get<ListType>(variable);
      std::string output = "[";
      for (size_t i = 0; i < list.size(); i++) {
        output += list[i].to_string();
        if (i != list.size() - 1) {
          output += ", ";
        }
      }
      output += "]";
      return output;
    }
    case Types::OBJECT: {
      ObjectType object = std::get<ObjectType>(variable);
      std::string output = "{";
      for (auto [k, v] : object) {
        std::string key_str = std::visit(
            [&k](auto &&arg) -> std::string {
              using DecayT = std::decay_t<decltype(arg)>;
              if constexpr (std::is_same_v<DecayT, int>)
                return std::to_string(std::get<int>(k));
              else if constexpr (std::is_same_v<DecayT, double>)
                return std::to_string(std::get<double>(k));
              else if constexpr (std::is_same_v<DecayT, std::string>)
                return "\"" + std::get<std::string>(k) + "\"";
              else if constexpr (std::is_same_v<DecayT, bool>)
                return (std::get<bool>(k) ? "True" : "False");
              else
                return "<unknown>";
            },
            k);
        output += key_str + ":";
        output += v.to_string();
        output += ",";
      }
      return output;
    }
    default:
      return "Unkown";
    }
    return "";
  }
};
// This will transform your arguments into a ListType that will be used for the
// Value
inline ListType List(ListInitializer v) { return ListType(v.begin(), v.end()); }
// This will transform your arguments into an ObjectType that will be used as a
// value
inline ObjectType Object(ObjectInitializer v) {

  return ObjectType(v.begin(), v.end());
}
}; // namespace Jinja

#endif