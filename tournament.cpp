////////////////////////////////////////////////////////////////////
/// File "tournament.cpp"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#include "auxFuncs.h"
#include "tournament.h"

using namespace GRGA;

int Tournament::tournament(pType * population)
{
  int winner = 0;

  for (int i = 0; i < p["tournamentSize"].i; i++)
    {
      int candidate = RANDOM(0, (int) population->size() - 1);
      if (candidate > winner)
        {
          winner = candidate;
        }
    }
  return winner;
}
