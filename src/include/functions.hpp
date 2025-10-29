#ifndef FUNCTIONS_JINJA
#define FUNCTIONS_JINJA

#include "values.hpp"
#include <functional>
#include <vector>

namespace Jinja {
// I Should define the basic functions here
using JinjaArguments = std::vector<Value>;
using JinjaFunction = std::function<Value(JinjaArguments)>;
// a small example
Value print(JinjaArguments args);
Value range(JinjaArguments args);
Value enumerate(JinjaArguments args);
} // namespace Jinja

#endif