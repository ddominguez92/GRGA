////////////////////////////////////////////////////////////////////
/// File "auxFuncs.h"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#ifndef AUXFUNCS_H
#define AUXFUNCS_H

#include <algorithm>
#include <iostream>
#include <random>
#include <sstream>
#include <stack>
#include <time.h>
#include "constants.h"
#include "parameters.h"
#include "types.h"

#define RANDOMEXCLUSIVE(low, high) (rand() % (high - 1 - low) + low + 1)
#define RANDOM(low, high) ((high < low) ? high : rand() % (high - low + 1) + low)

using namespace std;

namespace GRGA
{

  inline void printFormulaPostFix(fType * f)
  {
    cout << "Formula: ";
    unsigned int * cur = f->expression;
    eSymbol s;

    while (*cur != kEnd)
      {
        s = static_cast<eSymbol>(*cur);
        switch (s)
          {
          case kInd:
            cout << mStrings.at(s);
            cout << "(" << f->parameters[cur - f->expression] << ")";
            break;
          case kPrev:
            cout << mStrings.at(s);
            cout << "[" << f->prev[cur - f->expression] << "]";
            break;
          default:
            cout << mStrings.at(s);
            break;
          }
        cur++;
      }
    cout << "#" << endl;
  }

  inline void printFormula(fType * f)
  {

    printFormulaPostFix(f);

    std::ostringstream levels;
    std::ostringstream operators;
    std::ostringstream output;
    int level = 1;
    std::stack<eSymbol> ops;
    std::stack<unsigned int> rest;
    rest.push(1);
    output << "(";
    while(levels.tellp() < output.tellp())
      {
        levels << level;
        operators << ' ';
      }

    unsigned int *cur = &f->expression[f->length - 2];
    eSymbol s;

    while (cur >= f->expression)
      {
        s = static_cast<eSymbol>(*cur);
        if (s > kOperatorsBin && s < kOperatorsBinEnd)
          {
            ops.push(s);
            rest.push(2);
            output << "(";
            level++;
            levels << level;
            operators << ' ';
          }
        else
          {
            switch (s)
              {
              case kInd:
                output << mStrings.at(s);
                output.precision(12);
                output << "(" << f->parameters[cur - f->expression] << ")";
                break;
              case kPrev:
                output << mStrings.at(s);
                output << "[" << f->prev[cur - f->expression] << "]";
                break;
              default:
                output << mStrings.at(s);
                break;
              }
            while(levels.tellp() < output.tellp())
              {
                levels << level;
                operators << ' ';
              }
            rest.top()--;

            while((rest.size() > 0) && (rest.top() == 0))
              {
                ops.pop();
                rest.pop();
                output << ")";
                levels << level;
                operators << ' ';
                level--;
                if (rest.size() > 0)
                  rest.top()--;
              }
            if (!rest.empty() && rest.top() == 1)
              {
                s = ops.top();
                output << ")" << mStrings.at(s) << "(";
                levels << level;
                operators << ' ';
                while(levels.tellp() < ((int) output.tellp() - 1))
                  {
                    levels << level;
                    operators << '^';
                  }
                levels << level;
                operators << ' ';
              }
          }
        cur--;
      }

    output << "#";
    levels << 0;
    operators << '^';
    cout << output.str() << endl;
    cout << operators.str() << endl;
    cout << levels.str() << endl;
  }

  inline bool isOperator(unsigned int s)
  {
    return (s > kOperatorsBin) && (s < kOperatorsBinEnd);
  }

  inline int getRandomNode(fType * f)
  {
    int point = 1;
    int cutNode = RANDOM(1, (f->length)/2 - 1);
    while (cutNode > 0)
      {
        while (!isOperator(f->expression[point]))
          {
            point++;
          }
        point++;
        cutNode--;
      }
    point--;
    return point;
  }

  // Random utilities
  inline void initRand()
  {
    time_t seconds;
    time(&seconds);
    srand((unsigned int) seconds);
  }

  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::normal_distribution<> dist01(0.0, 1.0);

  // Cleaners
  inline void clearFType(fType * f)
  {
    free(f->expression);
    free(f->prev);
    free(f->parameters);
    delete f;
  }

  inline void clearIType(iType * i)
  {
    clearFType(i->formula);
    delete i;
  }

  inline void clearPType(pType * p)
  {
    for(auto it = p->begin(); it != p->end(); it++)
      {
        clearIType(* it);
      }
    delete p;
  }

  inline void clearIESType(iESType * i)
  {
    free(i->control);
    free(i->parameters);
    delete i;
  }

  inline bool compareIESType(iESType * first, iESType * second)
  {
    return (first->score < second->score);
  }

  inline bool compareIType(iType * first, iType * second)
  {
    return (first->score > second->score);
  }

  inline void sortPType(pType * p)
  {
    std::sort(p->begin(), p->end(), compareIType);
  }

}



#endif // AUXFUNCS_H
