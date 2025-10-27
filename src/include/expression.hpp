#ifndef EXPRESSION_JINJA
#define EXPRESSION_JINJA
#include "values.hpp"
#include <string>
#include <variant>
#include <vector>

namespace Jinja {
namespace Expression {

// lets first declare it
struct FunctionCalling;
struct ListCreation;
struct ExpressionCalling;
struct VariableCalling;
struct ValueCreation;

using ExpressionTypes =
    std::variant<VariableCalling, FunctionCalling, ListCreation, ValueCreation,
                 ExpressionCalling>;

struct FunctionCalling {
  std::string function{""};
  // i should think of various ways i could handle this
  // i will most possibly just assign a random argument to pass this to the
  // function
  // a | func
  // func(a,b,"c",[1,2,"asdf"])
  std::vector<ExpressionTypes> args;
};
// Okay this should be calling an devaluating the inside code

struct ListCreation {
  // [1,2,[1,2]]
  std::vector<ExpressionCalling> args;
};
struct ValueCreation {
  // string, int, float,bool
  Value variable = nullptr;
};
struct VariableCalling {
  // not quotes.
  // not special characters or numbers at the beginning
  // only allowed characters in the beginning are:
  // _,a-z,A-Z.
  std::string variable_name{""};
};
struct ExpressionCalling {
  // {{asdf | func}}
  // {{func([1,2,3])}}
  std::vector<ExpressionTypes> blocks;
};
} // namespace Expression
} // namespace Jinja

#endif
