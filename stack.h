////////////////////////////////////////////////////////////////////
/// File "stack.h"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#ifndef GRGA_STACK_H
#define GRGA_STACK_H

#include <stack>
#include "constants.h"

namespace GRGA
{
  template <typename type>
  class Stack
  {
  private:
    std::stack<type> * stack;
  public:
    ~Stack();
    Stack();
    void operate(eSymbol s);
    void ind(type f);
    type end();
  private:
    void add();
    void sub();
    void mul();
    void div();
    void pow();
    void log();
    void comp();
    void mod();
  };

}

#endif // GRGA_STACK_H
