#include "tokenizer.hpp"
#include <iostream>
#include <string>

int main() {

  std::string tmpl = R"({{asdf}}{#
hello world example
#} Hello {{name | "sup"}} {{ "[[{\\\"%%\"\\}]]" }})";
  std::cout << "CONTENT:\n" << tmpl << "\nEND\n\n";
  Jinja::Tokenizer tokenizer;
  try {

    auto out = tokenizer.Tokenize(tmpl);
    tokenizer.Print(out);
  } catch (std::string c) {
    std::cout << c << "\n";
  }
}