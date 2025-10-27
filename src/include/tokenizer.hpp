#ifndef TOKENIZER_JINJA
#define TOKENIZER_JINJA
#include <cstddef>
#include <string>
#include <variant>
#include <vector>

namespace Jinja {
// we define some basic tooling
enum class Token {
  // variable calling
  OPEN_COMMENT, // added
  END_COMMENT,  // added
  // {{variable|function}}
  OPEN_CALLING, // added
  END_CALLING,  // added
  // if,else,endif, for, foreach, etc
  // {%expr%}
  OPEN_EVAL, // added
  END_EVAL,  // added
  // to define dictionaries
  OPEN_CURLY_BRACKETS, // added
  END_CURLY_BRACKETS,  // added
  // to define lists or argument calling
  OPEN_SQUARED_BRACKETS, // added
  END_SQUARED_BRACKETS,  // added
  // parentesis for function calling
  OPEN_PARENTHESIS,
  END_PARENTHESIS,
  // rest of
  EQUALS_TO, // added
  PIPE,      // added
  COMMA,     // added
  // i am going to add this now
  DOUBLE_QUOTES, // Added
  SINGLE_QUOTES, // Added
  NOTHING,
};
using TokensOutput = std::variant<Token, std::string>;
class Tokenizer {
private:
  // comments
  std::string m_open_comment = "{#";
  std::string m_close_comment = "#}";
  // variable calling
  std::string m_open_calling = "{{";
  std::string m_end_calling = "}}";
  // expression calling
  std::string m_open_eval = "{%";
  std::string m_end_eval = "%}";

private:
  // this function will be used to avoid any exception, and to use it in a more
  // generic way so i can avoid writting repetitive code when tokenizing
  bool on_end(bool &inside_eval_code, size_t &index, const std::string &content,
              const std::string &open, const std::string &end,
              std::string &part, std::vector<TokensOutput> &tokens);
  // i dump the data that i have in the part which is going to be evaluated for
  // future expressions
  void add_and_clean_part(std::string &part, std::vector<TokensOutput> &tokens);
  // i manage the code tokens here.
  void special_code_tokens(std::string &part, std::vector<TokensOutput> &tokens,
                           Token &quote_type, bool &inside_string, char value);
  // I clean and parse the string
  bool special_string_tokens(std::string &part,
                             std::vector<TokensOutput> &tokens,
                             Token &quote_type, bool &inside_string, char value,
                             bool &escaped_char);

public:
  Tokenizer() {};
  std::vector<TokensOutput> Tokenize(const std::string &content);
  void SetCalling(const std::string &open_calling,
                  const std::string &end_calling);
  void SetEval(const std::string &open_eval, const std::string &end_eval);
  void Print(const std::vector<TokensOutput> &values);
};
} // namespace Jinja
#endif