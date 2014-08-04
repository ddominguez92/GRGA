////////////////////////////////////////////////////////////////////
/// File "gp.h"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#ifndef GENETICPROGRAMMING_H
#define GENETICPROGRAMMING_H

#include "types.h"

namespace GRGA
{
  namespace GeneticProgramming
  {
    fType * crossover(fType * p1, fType * p2);
    fType * mutateByRandomCrossover(fType * p1);
    fType * mutate(fType * p1);
    pType * initialPopulation();
  }
}

#endif // MUTATOR_H
