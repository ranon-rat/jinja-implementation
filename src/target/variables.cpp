#include "values.hpp"
// I love MODERN C++
int main() {
  Jinja::Value v = Jinja::Object({
      {"v", "14"},
      {"b", "13"},
      {
          "c",
          Jinja::Object({
              {"v", "14"},
              {"d", Jinja::List({
                        1,
                        2,
                        3,
                        Jinja::List({"a", "b"}),
                    })},
              {"c", nullptr},
              {12,nullptr},
          }),
      },
  });
  v.print();
}
