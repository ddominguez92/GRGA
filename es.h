///////////////////////////////////////////////////////////////////
/// File "es.h"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
///////////////////////////////////////////////////////////////////

#ifndef EVOLUTIONSTRATEGY_H
#define EVOLUTIONSTRATEGY_H

#include "types.h"

namespace GRGA
{
  namespace EvolutionStrategy
  {
    iESType * mutate(iESType * p1, float proportion);
    pESType * initialPopulation(fType * f, int age);
  }
}

#endif // EVOLUTIONSTRATEGY_H
