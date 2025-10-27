#include "lexic.hpp"
#include "statement.hpp"
#include "tokenizer.hpp"
#include <iostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace Jinja {

Parser::ExpressionTypesList
Parser::parse_expression(std::vector<TokensOutput>) {
  ExpressionTypesList output;
  return output;
}
Parser::CodeBlockTypesList
Parser::Parse(const std::vector<TokensOutput> &input) {
  CodeBlockTypesList output;
  for (const TokensOutput &token : input) {
    std::visit(
        [&token, &output, *this](auto &&arg) {
          using DecayT = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<DecayT, std::string>) {
            const std::string &value = std::get<std::string>(token);
            output.emplace_back(Statement::TextBlock{.text = value});
            std::cout << value << "\n";
          } else if constexpr (std::is_same_v<DecayT, Token>) {
            // so here is where i should be handling other things XD
            std::cout << "not implemented yet";
          }
        },
        token);
  }
  return output;
}

Parser::CodeBlockTypesList Parser::Parse(const std::string &input) {
  std::vector<TokensOutput> tokens = m_tokenizer.Tokenize(input);
  return Parse(tokens);
}

} // namespace Jinja
