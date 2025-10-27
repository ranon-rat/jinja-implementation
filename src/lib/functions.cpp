#include "functions.hpp"
#include "values.hpp"
#include <iostream>
#include <numeric>
#include <vector>

namespace Jinja {
Value print(JinjaArguments args) {
  // i receive the variables, then i compare them and i send an output
  int number = std::get<int>(args[0].variable);
  std::cout << number << "\n";
  return nullptr;
}
Value range(JinjaArguments args) {
  int begin = 0;
  int last = std::get<int>(args[0].variable);
  if (args.size() == 2) {
    last = std::get<int>(args[1].variable);
  }
  std::vector<Value> output(last - begin);
  std::iota(output.begin(), output.end(), begin);
  return nullptr;
}

Value enumerate(JinjaArguments args){
   std::vector<Value> output;
   for(int i=0;i<(int)args.size();i++){
    output.emplace_back(List({i,args[i]}));
   }
   // i love that you can do things like this and c++ will totally accept it
   return output;
}

} // namespace Jinja