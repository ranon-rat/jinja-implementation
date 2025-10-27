#include "lexic.hpp"
#include "expression.hpp"
#include "statement.hpp"
#include "tokenizer.hpp"
#include "utilities.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

namespace Jinja {
Token Parser::finish_token(const Token &initial) {

  switch (initial) {
  case Token::OPEN_CALLING:
    return Token::END_CALLING;
  case Token::OPEN_EVAL:
    return Token::END_EVAL;
  case Token::OPEN_CURLY_BRACKETS:
    return Token::END_CURLY_BRACKETS;
  case Token::OPEN_SQUARED_BRACKETS:
    return Token::END_SQUARED_BRACKETS;
  default:
    return initial;
  }
};

bool is_token(const TokensOutput token) {
  using DecayT = std::decay_t<decltype(token)>;
  return (std::is_same_v<DecayT, Token>);
}
// if true then break
void Parser::inside_quotes(const std::vector<TokensOutput> &input,
                           ExpressionTypesList &output, size_t &index) {
  using namespace Expression;
  ValueCreation new_expression;
  TokensOutput next_token = input[index++];
  std::string internal_string = "";

  // i can assume that the next token will be a quote
  if (!is_token(next_token)) {
    internal_string = std::get<std::string>(next_token);
    index++;
  }
  new_expression.variable = internal_string;
  output.emplace_back(new_expression);
}
WhatShoulDo Parser::inside_token(const std::vector<TokensOutput> &input,
                                 ExpressionTypesList &output, size_t &index,
                                 const Token, const Token finish) {
  using namespace Expression;
  // i get some previous values
  Token current_token = std::get<Token>(input[index]);
  if (current_token == finish)
    return BREAK;

  switch (current_token) {
  case Token::DOUBLE_QUOTES:
  case Token::SINGLE_QUOTES:
    inside_quotes(input, output, index);
    return CONTINUE;

  default:
    return CONTINUE;
  }
  return CONTINUE;
}

Parser::ExpressionTypesList
Parser::parse_expression(const std::vector<TokensOutput> &input, size_t &index,
                         const Token initial_token, const Token finish) {
  using namespace Expression;
  ExpressionTypesList output;

  for (size_t i = index; i < input.size(); i++) {
    TokensOutput current_token = input[i];
    if (is_token(current_token)) {
      auto should_do = (inside_token(input, output, i, initial_token, finish));
      if (should_do == BREAK)
        break;
      else if (should_do == CONTINUE)
        continue;
    }
    const std::string &value = std::get<std::string>(current_token);
    std::cout << value << "\n";
  }
  return output;
}
Parser::ExpressionTypesList
Parser::parse_expression(const std::vector<TokensOutput> &input, size_t &index,
                         const Token initial_token) {
  return parse_expression(input, index, initial_token,
                          finish_token(initial_token));
}
Parser::CodeBlockTypesList
Parser::Parse(const std::vector<TokensOutput> &input) {
  CodeBlockTypesList output;
  for (size_t i = 0; i < input.size(); i++) {
    const TokensOutput token = input[i];
    if (is_token(token)) {
      std::cout << "not implemented yet";
      continue;
    }
    const std::string &value = std::get<std::string>(token);
    output.emplace_back(Statement::TextBlock{.text = value});
    std::cout << value << "\n";
  }
  return output;
}

Parser::CodeBlockTypesList Parser::Parse(const std::string &input) {
  std::vector<TokensOutput> tokens = m_tokenizer.Tokenize(input);
  return Parse(tokens);
}

} // namespace Jinja
