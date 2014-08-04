////////////////////////////////////////////////////////////////////
/// File "es.h"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#ifndef GRGA_EVALUATION_H
#define GRGA_EVALUATION_H

#include "gnuplot_c.h"
#include "types.h"

namespace GRGA
{

  namespace Evaluation
  {
    extern h_GPC_Plot *h2DPlot;

    scorereal score(fType * formula, bool print = false);
    inline scorereal score(iESType * i)
    {
      xreal * tmp = i->formula->parameters;
      i->formula->parameters = i->parameters;
      scorereal res = score(i->formula);
      i->formula->parameters = tmp;
      return res;
    }

    xreal evaluate(fType * formula, unsigned int index);
    xreal evaluateFullSeries(fType * formula, unsigned int index,
                             xreal * prevs, int prevIndex);
  }

}

#endif // GRGA_EVALUATION_H
