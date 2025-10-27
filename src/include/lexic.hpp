#ifndef PARSER_JINJA
#define PARSER_JINJA
#include "expression.hpp"
#include "statement.hpp"
#include "tokenizer.hpp"
#include "utilities.hpp"
#include <cstddef>
#include <string>
#include <vector>

namespace Jinja {

class Parser {
  Tokenizer m_tokenizer;

public:
  using ExpressionTypesList = std::vector<Expression::ExpressionTypes>;
  using CodeBlockTypesList = std::vector<Statement::CodeBlockTypes>;

private:
  bool is_token(const TokensOutput token);
  // if true then break
  void inside_quotes(const std::vector<TokensOutput> &input,
                     ExpressionTypesList &output, size_t &index);

  WhatShoulDo inside_token(const std::vector<TokensOutput> &input,
                           ExpressionTypesList &output, size_t &index,
                         const Token finish);
// here i am managing the variable calling or the function calling, or the value creation
  void not_in_token(const std::vector<TokensOutput> &input,
                    ExpressionTypesList &output, size_t &index,
                const Token finish);
  Token finish_token(const Token &initial);
  Expression::ExpressionCalling parse_expression(const std::vector<TokensOutput> &input,
                                       size_t &index,
                                       const Token finish);
  

public:
  Parser() {};
  CodeBlockTypesList Parse(const std::string &tmpl);
  CodeBlockTypesList Parse(const std::vector<TokensOutput> &input);
  void Print();
};

} // namespace Jinja

#endif