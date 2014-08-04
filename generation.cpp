////////////////////////////////////////////////////////////////////
/// File "generation.cpp"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#include <random>
#include <stack>
#include "auxFuncs.h"
#include "constants.h"
#include "generation.h"
#include "parameters.h"

using namespace GRGA;

fType * Generation::generate()
{
  unsigned int * auxf = (unsigned int *)malloc(sizeof(unsigned int) * p["maxFLength"].i);
  int auxfIndex = 0;
  unsigned int * f = (unsigned int *)malloc(sizeof(unsigned int) * p["maxFLength"].i);
  int fIndex = 0;
  unsigned int * prev = (unsigned int *)malloc(sizeof(unsigned int) * p["maxFLength"].i);
  unsigned int * auxp = (unsigned int *)malloc(sizeof(unsigned int) * p["maxFLength"].i);
  xreal * var = (xreal *)malloc(sizeof(xreal) * p["maxFLength"].i);
  xreal * auxv = (xreal *)malloc(sizeof(xreal) * p["maxFLength"].i);

  int lvl = 0;
  std::stack<int> * stack = new std::stack<int>();
  stack->push(1);

  bool leaf;

  while (!stack->empty())
    {
      leaf = ((xreal) rand() / RAND_MAX) < p["probLeaf"].f;
      leaf |= ( lvl >= p["maxDepth"].i );
      stack->top()--;
      lvl++;
      if (leaf)
        {
          auxf[auxfIndex] = RANDOMEXCLUSIVE(kOperands, kOperandsEnd);
          stack->push(0);
          switch (auxf[auxfIndex]) {
            case kInd:
              auxv[auxfIndex] = p["indInitVal"].x;
              break;
            case kPrev:
              if (p["maxPrev"].i < 1)
                auxf[auxfIndex] = kTime;
              else
                auxp[auxfIndex] = RANDOM(1, p["maxPrev"].i);
              break;
            default:
              break;
            }
        }
      else
        {
          auxf[auxfIndex] = RANDOMEXCLUSIVE(kOperatorsBin, kOperatorsBinEnd);
          stack->push(2);
        }

      while(!stack->empty() && stack->top() == 0)
        {
          stack->pop();
          lvl--;
        }
      auxfIndex++;
    }

  auxfIndex--;
  while (auxfIndex >= 0)
    {
      f[fIndex] = auxf[auxfIndex];
      var[fIndex] = auxv[auxfIndex];
      prev[fIndex] = auxp[auxfIndex];

      auxfIndex--;
      fIndex++;
    }
  f[fIndex] = kEnd;
  fIndex++;


  fType * fS = new fType();

  fS->expression = f;
  fS->prev = prev;
  fS->parameters = var;
  fS->length = fIndex;
  fS->maxLength = p["maxFLength"].i;

  free(auxf);
  free(auxv);
  free(auxp);
  delete stack;

  return fS;
}
