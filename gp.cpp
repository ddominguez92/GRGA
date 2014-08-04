////////////////////////////////////////////////////////////////////
/// File "gp.cpp"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#include <stack>
#include <string.h>
#include "auxFuncs.h"
#include "constants.h"
#include "generation.h"
#include "gp.h"
#include "parameters.h"

using namespace std;
using namespace GRGA;

fType * GeneticProgramming::crossover(fType * p1, fType * p2)
{
  int maxFLength = p["maxFLength"].i;
  int maxDepth = p["maxDepth"].i;
  xreal indInitVal = p["indInitVal"].x;
  int maxPrev = p["maxPrev"].i;

  unsigned int * auxf = (unsigned int *)malloc(sizeof(unsigned int) * maxFLength);
  int auxfIndex = 0;
  unsigned int * f = (unsigned int *)malloc(sizeof(unsigned int) * maxFLength);
  int fIndex = 0;
  unsigned int * prev = (unsigned int *)malloc(sizeof(unsigned int) * maxFLength);
  unsigned int * auxp = (unsigned int *)malloc(sizeof(unsigned int) * maxFLength);
  xreal * var = (xreal *)malloc(sizeof(xreal) * maxFLength);
  xreal * auxv = (xreal *)malloc(sizeof(xreal) * maxFLength);

  int p1fIndex = getRandomNode(p1);
  int p1auxfIndex = p1->length - 2;
  int p2fIndex = getRandomNode(p2);

  int lvl = 0;
  std::stack<int> * stack = new std::stack<int>();
  stack->push(1);

  bool leaf;

  eSymbol s;

  while (!stack->empty() && p1auxfIndex != p1fIndex)
    {
      auxf[auxfIndex] = p1->expression[p1auxfIndex];
      auxp[auxfIndex] = p1->prev[p1auxfIndex];
      auxv[auxfIndex] = p1->parameters[p1auxfIndex];

      stack->top()--;
      lvl++;

      s = static_cast<eSymbol>(auxf[auxfIndex]);
      stack->push(mParams.at(s) + 1);

      while(!stack->empty() && stack->top() == 0)
        {
          stack->pop();
          lvl--;
        }

      auxfIndex++;
      p1auxfIndex--;
    }

  int stackCounter = 1;

  while (stackCounter > 0 && p1fIndex >= 0)
    {
      s = static_cast<eSymbol>(p1->expression[p1fIndex]);

      stackCounter += mParams.at(s);
      p1fIndex--;
    }

  stackCounter = 1;
  bool finished = false;
  int params;

  while ((!stack->empty()) && (stackCounter > 0) && !finished)
    {
      stack->top()--;
      lvl++;

      leaf = lvl - 1 >= maxDepth;
      leaf = leaf && isOperator(p2->expression[p2fIndex]);

      if (leaf)
        {
          auxf[auxfIndex] = RANDOMEXCLUSIVE(kOperands, kOperandsEnd);
          stack->push(0);
          switch (auxf[auxfIndex])
            {
            case kInd:
              auxv[auxfIndex] = indInitVal;
              break;
            case kPrev:
              if (maxPrev < 1)
                auxf[auxfIndex] = kInd;
              else
                auxp[auxfIndex] = RANDOM(1, maxPrev);
              break;
            default:
              break;
            }

          p2fIndex--;
          int skipStackCounter = 1;
          while (skipStackCounter > 0)
            {
              s = static_cast<eSymbol>(p2->expression[p2fIndex]);
              skipStackCounter += mParams.at(s);
              p2fIndex--;
            }

          s = static_cast<eSymbol>(auxf[auxfIndex]);
          params = mParams.at(s);
          stackCounter += params;

        }
      else
        {
          auxf[auxfIndex] = p2->expression[p2fIndex];
          auxp[auxfIndex] = p2->prev[p2fIndex];
          auxv[auxfIndex] = p2->parameters[p2fIndex];

          s = static_cast<eSymbol>(auxf[auxfIndex]);
          params = mParams.at(s);
          stack->push(params + 1);

          stackCounter += params;
        }

      auxfIndex++;
      p2fIndex--;

      if(stackCounter == 0)
        {
          finished = true;
          break;
        }

      while(!stack->empty() && stack->top() == 0)
        {
          stack->pop();
          lvl--;
        }

    }

  while (p1fIndex >= 0)
    {
      auxf[auxfIndex] = p1->expression[p1fIndex];
      auxp[auxfIndex] = p1->prev[p1fIndex];
      auxv[auxfIndex] = p1->parameters[p1fIndex];

      auxfIndex++;
      p1fIndex--;
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
  fS->maxLength = maxFLength;

  free(auxf);
  free(auxv);
  free(auxp);
  delete stack;

  return fS;
}


fType * GeneticProgramming::mutateByRandomCrossover(fType * p1)
{
  fType * p2 = Generation::generate();
  fType * c = crossover(p1, p2);

  clearFType(p2);
  return c;
}

fType * GeneticProgramming::mutate(fType * p1)
{
  auto child = new fType;
  int length = p1->length;

  child->expression = (unsigned int *)malloc(sizeof(unsigned int) * p["maxFLength"].i);
  memcpy(child->expression, p1->expression, length * sizeof(unsigned int));
  child->prev = (unsigned int *)malloc(sizeof(unsigned int) * p["maxFLength"].i);
  memcpy(child->prev, p1->prev, length * sizeof(unsigned int));
  child->parameters = (xreal *)malloc(sizeof(xreal) * p["maxFLength"].i);
  memcpy(child->parameters, p1->parameters, length * sizeof(xreal));

  child->length = length;
  child->maxLength = p1->maxLength;

  int mutationPoint = RANDOM(0, length - 2);

  eSymbol s = static_cast<eSymbol> (child->expression[mutationPoint]);
  if ((s > kOperatorsBin) && (s < kOperatorsBinEnd))
    {
      child->expression[mutationPoint] = RANDOMEXCLUSIVE(kOperatorsBin, kOperatorsBinEnd);
    }
  else if ((s > kOperands) && (s < kOperandsEnd))
    {
      s = static_cast<eSymbol> (RANDOMEXCLUSIVE(kOperands, kOperandsEnd));
      child->expression[mutationPoint] = s;
      switch (s) {
        case kInd:
          child->parameters[mutationPoint] = p["indInitVal"].x;
          break;
        case kPrev:
          if (p["maxPrev"].i < 1)
            child->expression[mutationPoint] = kTime;
          else
            child->prev[mutationPoint] = RANDOM(1, p["maxPrev"].i);
          break;
        default:
          break;
        }
    }

  return child;

}


pType * GeneticProgramming::initialPopulation()
{
  auto pop = new pType;
  pop->reserve(p["populationSize"].i);

  for (int i = 0; i < p["populationSize"].i; i++)
    {
      auto individual = new iType;
      individual->formula = Generation::generate();
      individual->score = -1;
      pop->push_back(individual);
    }

  return pop;
}
