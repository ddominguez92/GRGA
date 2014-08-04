////////////////////////////////////////////////////////////////////
/// File "plot.h"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#ifndef GRGA_PLOT_H
#define GRGA_PLOT_H

#include "types.h"

namespace GRGA
{
  namespace Plot
  {
    void initializePlots();
    void closePlots();
    void plotIteration(int i, scorereal *scores, scorereal *thresholdBar, scorereal *lengths, iType *bestind);
    void plotFinalResults(int i, scorereal *scores, scorereal *thresholdBar, scorereal *lengths, iType *bestind);
    void plotEvaluation(xreal *results);
  }

}

#endif // GRGA_PLOT_H
