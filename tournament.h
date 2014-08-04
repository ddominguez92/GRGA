////////////////////////////////////////////////////////////////////
/// File "tournaments.h"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#ifndef GRGA_TOURNAMENT_H
#define GRGA_TOURNAMENT_H

#include "types.h"

namespace GRGA
{
  namespace Tournament
  {
    // Returns the index of the winner
    int tournament(pType * population);
  }
}

#endif // GRGA_TOURNAMENT_H
