#include "lexic.hpp"
#include "expression.hpp"
#include "statement.hpp"
#include "tokenizer.hpp"
#include "utilities.hpp"
#include "values.hpp"
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
  case Token::OPEN_PARENTHESIS:
    return Token::END_PARENTHESIS;
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
                                 const Token finish) {
  using namespace Expression;
  // i get some previous values
  Token current_token = std::get<Token>(input[index]);
  if (current_token == finish || current_token == Token::COMMA)
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

void Parser::not_in_token(const std::vector<TokensOutput> &input,
                          ExpressionTypesList &output, size_t &index,
                          const Token finish) {
  using namespace Expression;
  const std::string &cell = std::get<std::string>(input[index]);
  const char first_char = cell[0];
  if (first_char >= '0' && first_char <= '9') {
    ValueCreation new_expression;
    Value value;
    // that means that its a number btw
    if (cell.find(".")) {
      value = std::stod(cell);
    } else {
      value = std::stoi(cell);
    }
    new_expression.variable = value;
    output.emplace_back(new_expression);
    return;
  }
  if (cell == "True") {
    output.emplace_back(ValueCreation{.variable = true});
    return;
  }
  if (cell == "False") {
    output.emplace_back(ValueCreation{.variable = false});
    return;
  }
  // obviamente el siguiente token sabra si es el final
  const Token next_token = std::get<Token>(input[index++]);
  if (next_token == finish) {
    // so i just insert the variable name, and i continue with my life :)
    output.emplace_back(VariableCalling{.variable_name = cell});
    return;
  }
  if (next_token == Token::OPEN_PARENTHESIS) {
    /// tengo que pensar sobre esto.
    std::string variable_name = cell;
    std::vector<ExpressionCalling> arguments;
    // vamos a pensar un poco uhhh
    Token current = next_token;
    while (current != finish&& current !=Token::COMMA) {
      arguments.emplace_back(
          parse_expression(input, index, Token::END_PARENTHESIS));
    }
    output.emplace_back(
        FunctionCalling{.function = variable_name, .args = arguments} );
  }
}
Expression::ExpressionCalling
Parser::parse_expression(const std::vector<TokensOutput> &input, size_t &index,
                         const Token finish) {
  using namespace Expression;
  ExpressionCalling output;

  for (size_t &i = index; i < input.size(); i++) {
    TokensOutput current_token = input[i];
    if (is_token(current_token)) {
      auto should_do = (inside_token(input, output.blocks, i, finish));
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
