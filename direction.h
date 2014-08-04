////////////////////////////////////////////////////////////////////
/// File "direction.h"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#ifndef GRGA_DIRECTION_H
#define GRGA_DIRECTION_ H

#include "gnuplot_c.h"
#include "types.h"

namespace GRGA
{

  namespace Direction
  {
    static h_GPC_Plot *h2DPlot = NULL;

    scorereal run();
    pType * deriveNextGeneration(pType *population);
    iType * runES(iType * f);
    void interruptHandler(int s);
  }

}

#endif // GRGA_DIRECTION_H
