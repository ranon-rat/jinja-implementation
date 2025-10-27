#include "tokenizer.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace Jinja {

std::vector<TokensOutput> Tokenizer::Tokenize(const std::string &content) {
  std::vector<TokensOutput> tokens;
  bool inside_eval_code = false;
  bool inside_comment = false;
  bool inside_string = false;
  bool escaped_char = false;
  std::string part;
  // token size
  size_t comment_size = m_open_comment.size();

  Token quote_type = Token::NOTHING;
  // if its inside then i will avoid adding the spaces or the endlines
  for (size_t i = 0; i < content.size(); ++i) {
    // i must avoid adding the comments pre parsing
    if (i + comment_size <= content.size()) {
      std::string possible_outcome(content.begin() + i,
                                   content.begin() + i + comment_size);
      if (possible_outcome == m_open_comment) {
        inside_comment = true;
        add_and_clean_part(part, tokens);
        i += comment_size - 1;
        continue;
      }
      if (possible_outcome == m_close_comment) {
        inside_comment = false;
        i += comment_size - 1;
        continue;
      }
    }
    if (inside_comment)
      continue;
    if (!inside_string) {

      if (on_end(inside_eval_code, i, content, m_open_calling, m_end_calling,
                 part, tokens))
        continue;

      if (on_end(inside_eval_code, i, content, m_open_eval, m_end_eval, part,
                 tokens))
        continue;
    }

    char value = content[i];
    if (inside_eval_code) {
      if (!inside_string) {
        special_code_tokens(part, tokens, quote_type, inside_string, value);
        continue;
      }
      if (special_string_tokens(part, tokens, quote_type, inside_string, value,
                                escaped_char))
        continue;
    }
    part += value;
  }
  add_and_clean_part(part, tokens);
  return tokens;
};

void Tokenizer::add_and_clean_part(std::string &part,
                                   std::vector<TokensOutput> &tokens) {
  if (part.size() == 0)
    return;

  tokens.emplace_back(part);
  part = "";
}
bool Tokenizer::on_end(bool &inside_eval_code, size_t &index,
                       const std::string &content, const std::string &open,
                       const std::string &end, std::string &part,
                       std::vector<TokensOutput> &tokens) {
  const size_t brackets_size = open.size();
  Token type_open =
      open == m_open_eval ? Token::OPEN_EVAL : Token::OPEN_CALLING;
  Token type_end = end == m_end_eval ? Token::END_EVAL : Token::END_CALLING;

  if (index + brackets_size <= content.size()) {
    std::string possible_outcome(content.begin() + index,
                                 content.begin() + index + brackets_size);

    if (possible_outcome == open) {
      if (inside_eval_code)
        throw "You cannot open another evaluation block";

      inside_eval_code = true;
      add_and_clean_part(part, tokens);
      index += brackets_size - 1;

      tokens.emplace_back(type_open);
      return true;
    }
    if (possible_outcome == end) {
      if (!inside_eval_code)
        throw "You cannot close an unexistent block.";
      inside_eval_code = false;
      add_and_clean_part(part, tokens);
      index += brackets_size - 1;
      tokens.emplace_back(type_end);
      return true;
    }
  }
  return false;
};

void Tokenizer::special_code_tokens(std::string &part,
                                    std::vector<TokensOutput> &tokens,
                                    Token &quote_type, bool &inside_string,
                                    char value) {

  switch (value) {
  case '\t':
  case '\n':
  case ' ':
    add_and_clean_part(part, tokens);
    return;
  case '=':
    add_and_clean_part(part, tokens);
    tokens.emplace_back(Token::EQUALS_TO);
    return;
  case ',':
    add_and_clean_part(part, tokens);
    tokens.emplace_back(Token::COMMA);
    return;
  case '|':
    add_and_clean_part(part, tokens);
    tokens.emplace_back(Token::PIPE);
    return;
  case '[':
    add_and_clean_part(part, tokens);
    tokens.emplace_back(Token::OPEN_SQUARED_BRACKETS);
    return;
  case ']':
    add_and_clean_part(part, tokens);
    tokens.emplace_back(Token::END_SQUARED_BRACKETS);
    return;
  case '{':
    add_and_clean_part(part, tokens);
    tokens.emplace_back(Token::OPEN_CURLY_BRACKETS);
    return;
  case '}':
    add_and_clean_part(part, tokens);
    tokens.emplace_back(Token::END_CURLY_BRACKETS);
    return;
  case '(':
    add_and_clean_part(part, tokens);
    tokens.emplace_back(Token::OPEN_PARENTHESIS);
    return;
  case ')':
    add_and_clean_part(part, tokens);
    tokens.emplace_back(Token::END_PARENTHESIS);
    return;
    ;
  case '"':
    add_and_clean_part(part, tokens);
    tokens.emplace_back(Token::DOUBLE_QUOTES);
    quote_type = Token::DOUBLE_QUOTES;
    inside_string = true;
    return;
    ;
  case '\'':
    add_and_clean_part(part, tokens);
    tokens.emplace_back(Token::SINGLE_QUOTES);
    quote_type = Token::SINGLE_QUOTES;
    inside_string = true;
    return;
    ;
  case '\\':
    throw "Invalid character";
  default:
    break;
  }
  part += value;
}

bool Tokenizer::special_string_tokens(std::string &part,
                                      std::vector<TokensOutput> &tokens,
                                      Token &quote_type, bool &inside_string,
                                      char value, bool &escaped_char) {
  bool output = false;
  switch (value) {
  case '\\':
    if (!escaped_char) {
      escaped_char = true;
      output = true;
      return true;
    }
    break;
  case 'n':
    if (escaped_char) {
      part += '\n';
      escaped_char = false;
      output = true;
    }
    break;
  case 't':
    if (escaped_char) {
      part += '\t';
      escaped_char = false;
      output = true;
    }
    break;
  case '"':
    if (quote_type == Token::DOUBLE_QUOTES && !escaped_char) {
      add_and_clean_part(part, tokens);
      tokens.emplace_back(Token::DOUBLE_QUOTES);
      quote_type = Token::NOTHING;
      inside_string = false;
      output = true;
    }
    break;
  case '\'':
    if (quote_type == Token::SINGLE_QUOTES && !escaped_char) {
      add_and_clean_part(part, tokens);
      tokens.emplace_back(Token::SINGLE_QUOTES);
      quote_type = Token::NOTHING;
      inside_string = false;
      output = true;
    }
    break;
  }
  escaped_char = false;
  return output;
}

void Tokenizer::SetCalling(const std::string &open_calling,
                           const std::string &end_calling) {
  if (open_calling.size() != end_calling.size())
    throw "both arguments must have the same size";

  m_open_calling = open_calling;
  m_end_calling = end_calling;
};
void Tokenizer::SetEval(const std::string &open_eval,
                        const std::string &end_eval) {
  if (open_eval.size() != end_eval.size())
    throw "both arguments must have the same size";

  m_open_eval = open_eval;
  m_end_eval = end_eval;
};

void Tokenizer::Print(const std::vector<TokensOutput> &values) {
  for (auto v : values) {
    std::visit(
        [&v](auto &&arg) {
          using DecayT = std::decay_t<decltype(arg)>;
          if constexpr (std::is_same_v<DecayT, Token>) {
            Token t = std::get<Token>(v);
            switch (t) {
            case Token::OPEN_CURLY_BRACKETS:
              std::cout << "T{ ";
              break;
            case Token::END_CURLY_BRACKETS:
              std::cout << "}T ";
              break;
            case Token::OPEN_SQUARED_BRACKETS:
              std::cout << "T[ ";
              break;
            case Token::END_SQUARED_BRACKETS:
              std::cout << "]T ";
              break;
            case Token::OPEN_EVAL:
              std::cout << "OPEN_EVAL";
              break;
            case Token::END_EVAL:
              std::cout << "END_EVAL";
              break;
            case Token::OPEN_CALLING:
              std::cout << "OPEN_CALLING";
              break;
            case Token::END_CALLING:
              std::cout << "END_CALLING";
              break;
            case Token::PIPE:
              std::cout << "PIPE";
              break;
            case Token::DOUBLE_QUOTES:
              std::cout << "DOUBLE_QUOTES";
              break;

            default:
              break;
            }
          } else if constexpr (std::is_same_v<DecayT, std::string>) {
            std::cout << "\"" << std::get<std::string>(v) << "\" ";
          }
        },
        v);
    std::cout << ", ";
  }
  std::cout << "\n";
}
} // namespace Jinja