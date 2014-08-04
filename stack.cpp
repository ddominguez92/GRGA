////////////////////////////////////////////////////////////////////
/// File "stack.cpp"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include "stack.h"

using namespace GRGA;

template <typename type>
Stack<type>::Stack()
{
  stack = new std::stack<type>();
}

template <typename type>
Stack<type>::~Stack()
{
  delete(stack);
}

template <typename type>
void Stack<type>::operate(eSymbol s)
{
  switch (s)
    {
    case kAdd:
      add();
      break;
    case kSub:
      sub();
      break;
    case kMul:
      mul();
      break;
    case kDiv:
      div();
      break;
    case kPow:
      pow();
      break;
    case kLog:
      log();
      break;
    case kComp:
      comp();
      break;
    case kMod:
      mod();
      break;
    default:
      std::cout << "Unrecognised operand" << std::endl;
      exit(-1);
    }
}

template <typename type>
void Stack<type>::add()
{
  type op1 = stack->top();
  stack->pop();
  type op2 = stack->top();
  stack->pop();
  stack->push(op1 + op2);
}

template <typename type>
void Stack<type>::sub()
{
  type op1 = stack->top();
  stack->pop();
  type op2 = stack->top();
  stack->pop();
  stack->push(op1 - op2);
}

template <typename type>
void Stack<type>::mul()
{
  type op1 = stack->top();
  stack->pop();
  type op2 = stack->top();
  stack->pop();
  stack->push(op1 * op2);
}

template <typename type>
void Stack<type>::div()
{
  type op1 = stack->top();
  stack->pop();
  type op2 = stack->top();
  stack->pop();
  if (op2 == 0)
    stack->push(op1 == 0);
  else if (op1 == 0)
    stack->push(1.0);
  else
    stack->push(op1 / op2);
}

template <typename type>
void Stack<type>::pow()
{
  type op1 = stack->top();
  stack->pop();
  type op2 = stack->top();
  stack->pop();
  type res = std::pow(op1, op2);
  if ( std::isnan( res ))
    stack->push(0);
  else
    stack->push(res);
}

template <typename type>
void Stack<type>::log()
{
  type op1 = stack->top();
  stack->pop();
  type op2 = stack->top();
  stack->pop();
  type res = std::log2(op1) / std::log2(op2);
  if ( std::isnan( res ))
    stack->push(0);
  else
    stack->push(res);
}

template <typename type>
void Stack<type>::comp()
{
  type op1 = stack->top();
  stack->pop();
  type op2 = stack->top();
  stack->pop();
  stack->push((op1 > op2) ? 1.0 : 0.0);
}

template <typename type>
void Stack<type>::mod()
{
  type op1 = floor(stack->top());
  stack->pop();
  type op2 = floor(stack->top());
  stack->pop();
  if (op2 <= 0 || std::isinf( op2 ) || std::isinf( op1 ))
    {
      stack->push(0.0);
    }
  else
    {
      type res = (int) op1 % (int) op2;
      res = (res < 0) ? res + op2 : res;
      stack->push(res);
    }
}

template <typename type>
void Stack<type>::ind(type f)
{
  stack->push(f);
}

template <typename type>
type Stack<type>::end()
{
  type ret = stack->top();
  stack->pop();
  if (!stack->empty())
    {
      std::cout << "Formula finished with " << stack->size() << " items left" << std::endl;
      exit(-1);
    }

  if ( std::isinf( ret ))
    return 0;

  if ( std::isnan( ret ))
    return 0;

  return ret;
}

template class Stack<float>;
template class Stack<double>;
template class Stack<int>;
