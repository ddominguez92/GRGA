////////////////////////////////////////////////////////////////////
/// File "evaluation.cpp"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#include <cmath>
#include <limits>
#include "auxFuncs.h"
#include "constants.h"
#include "evaluation.h"
#include "stack.h"
#include "plot.h"

using namespace GRGA;
using namespace std;

h_GPC_Plot * Evaluation::h2DPlot = NULL;
scorereal Evaluation::score(fType * formula, bool print)
{
  int maxPrev = p["maxPrev"].i;
  int errorFunc = p["errorFunction"].i;

  xreal results[p["currentTargetLength"].i - maxPrev];
  scorereal error = 0;
  if (p["evaluationMode"].i == kEvalSinglePredict)
    {
      for (int i = maxPrev; i < p["currentTargetLength"].i; i++)
        {
          xreal result = evaluate(formula, i);
          xreal expected = targetData->data[i];

          scorereal localError;
          if (errorFunc == kEFuncRel)
            {
              if (expected == 0)
                localError = fabs(result);
              else
                localError = fabs((result - expected) / expected);
            }
          else
            {
              localError = fabs(result - expected);
            }

          if (p["errorMode"].i == kErrorMean)
            error += localError;
          else
            error = max(error, localError);

          results[i - maxPrev] = result;
        }
      error = error / p["currentTargetLength"].i;

    }
  else
    {
      xreal previous[maxPrev];
      int pIndex = 0;
      for (int i = 0; i < maxPrev; i++)
        previous[i] = targetData->data[i];

      int errorMode = p["errorMode"].i;

      for (int i = maxPrev; i < p["currentTargetLength"].i; i++)
        {
          xreal result = evaluateFullSeries(formula, i, previous, pIndex);
          xreal expected = targetData->data[i];

          scorereal localError;
          if (errorFunc == kEFuncRel)
            {
              if (expected == 0)
                localError = fabs(result);
              else
                localError = fabs((result - expected) / expected);
            }
          else
            {
              localError = fabs(result - expected);
            }

          if (errorMode == kErrorMean)
            error += localError;
          else
            error = max(error, localError);

          if (maxPrev > 0)
            {
              previous[pIndex] = result;
              pIndex++;
              pIndex %= maxPrev;
            }

          results[i - maxPrev] = result;
        }
      if (errorMode == kErrorMean)
        error = error / p["currentTargetLength"].i;
    }


  if (print)
    {
      Plot::plotEvaluation(results);
    }

  if ( std::isinf( error ))
  {
    return std::numeric_limits<double>::max();
  }
  if ( std::isnan( error ))
  {
    return std::numeric_limits<double>::max();
  }
  return error;
}

xreal Evaluation::evaluate(fType * formula, unsigned int index)
{
  auto stack = new Stack<xreal>;
  eSymbol s;
  int time = 0;
  unsigned int * fCursor = formula->expression;
  xreal * vCursor= formula->parameters;
  unsigned int * pCursor= formula->prev;

  while (*fCursor != kEnd)
    {
      s = static_cast<eSymbol>(*fCursor);
      switch (s)
        {
        case kInd:
          stack->ind(*vCursor);
          break;
        case kPrev:
          stack->ind(targetData->data[index - *pCursor]);
          break;
        case kTime:
          stack->ind(index);
          break;
        case k1:
          stack->ind(1.0);
          break;
        case k0:
          stack->ind(0.0);
          break;
        case kneg1:
          stack->ind(-1.0);
          break;
        default:
          stack->operate(s);
          break;
          break;
        }
      fCursor++;
      pCursor++;
      vCursor++;
      time++;
    }
  xreal result = stack->end();

  delete stack;
  return result;
}

xreal Evaluation::evaluateFullSeries(fType * formula, unsigned int index,
                                     xreal * prevs, int prevIndex)
{
  auto stack = new Stack<xreal>;
  eSymbol s;
  int time = 0;
  unsigned int * fCursor = formula->expression;
  xreal * vCursor= formula->parameters;
  unsigned int * pCursor= formula->prev;

  while (*fCursor != kEnd)
    {
      s = static_cast<eSymbol>(*fCursor);
      switch (s)
        {
        case kInd:
          stack->ind(*vCursor);
          break;
        case kPrev:
          stack->ind(prevs[ (prevIndex + p["maxPrev"].i -
              *pCursor) % p["maxPrev"].i ]);
          break;
        case kTime:
          stack->ind(index);
          break;
        case k1:
          stack->ind(1.0);
          break;
        case k0:
          stack->ind(0.0);
          break;
        case kneg1:
          stack->ind(-1.0);
          break;
        default:
          stack->operate(s);
          break;
          break;
        }
      fCursor++;
      pCursor++;
      vCursor++;
      time++;
    }
  xreal result = stack->end();

  delete stack;
  return result;
}

