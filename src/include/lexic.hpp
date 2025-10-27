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
  WhatShoulDo inside_token(std::vector<TokensOutput> input, ExpressionTypesList output, size_t &index,
                                       const Token initial_token);
  Token finish_token(const Token &initial);
  ExpressionTypesList parse_expression(std::vector<TokensOutput>, size_t &index,
                                       const Token initial_token);

public:
  Parser() {};
  CodeBlockTypesList Parse(const std::string &tmpl);
  CodeBlockTypesList Parse(const std::vector<TokensOutput> &input);
  void Print();
};

} // namespace Jinja

#endif