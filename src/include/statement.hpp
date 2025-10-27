#ifndef STATEMENT_JINJA
#define STATEMENT_JINJA
#include "expression.hpp"
#include "values.hpp"

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace Jinja {
namespace Statement {
// lets define the code blocks first
struct CodeBlockInterface;
struct TextBlock;
struct ForBlock;
struct IfBlock;
// then we are going to define a variant
using CodeBlockTypes =
    std::variant<Expression::ExpressionCalling, TextBlock, CodeBlockInterface>;
    // lets just define the special keywords that we have avaible
static std::unordered_map<std::string, bool> special_keywords={
    {"for",true},
    {"if", true},
};
// here is a simple interface that i will possibly be using
struct CodeBlockInterface {
  CodeBlockInterface *parent = nullptr;
  std::vector<CodeBlockTypes> code;
  std::unordered_map<std::string, Value> variables;
};
struct TextBlock {
  std::string text{""};
};
struct IfBlock : CodeBlockInterface {
  Expression::ExpressionCalling boolean;
};
using ForBlockRangeType =
    std::variant<Expression::ValueCreation, Expression::FunctionCalling,
                 Expression::ExpressionCalling>;
struct ForBlock {
  // for i in range(10)
  // for v in [1,2,3] )
  //   // for [a,b,c] in
  ForBlockRangeType range;
};

} // namespace Statement
}; // namespace Jinja

#endif