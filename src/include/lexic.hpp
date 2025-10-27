#ifndef PARSER_JINJA
#define PARSER_JINJA
#include "expression.hpp"
#include "statement.hpp"
#include "tokenizer.hpp"
#include <string>
#include <vector>

namespace Jinja {

class Parser {
  Tokenizer m_tokenizer;

public:
  using ExpressionTypesList = std::vector<Expression::ExpressionTypes>;
  using CodeBlockTypesList = std::vector<Statement::CodeBlockTypes>;

private:
  ExpressionTypesList parse_expression(std::vector<TokensOutput>);

public:
  Parser() {};
  CodeBlockTypesList Parse(const std::string& tmpl);
  CodeBlockTypesList Parse(const std::vector<TokensOutput>&input);
  void Print();
};

} // namespace Jinja

#endif