////////////////////////////////////////////////////////////////////
/// File "parameters.h"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#ifndef GRGA_PARAMETERS_H
#define GRGA_PARAMETERS_H

#include <map>
#include "constants.h"
#include "types.h"

namespace GRGA
{
  extern int GRGA_PLOT_RESULTS;
  extern int GRGA_PLOT_RESULTS_TO_FILE;

  extern int LOG_TO_FILE;
  extern int OMIT_LOG;

  typedef union {
    float f;
    xreal x;
    scorereal s;
    int i;
  } parameter;

  extern std::map<std::string, parameter> p;

  namespace Parameters
  {
    void loadParametersFromFile(std::string fileName);
    void generateDerivedParameters();
  }

  extern dType * targetData;


}

#endif // GRGA_PARAMETERS_H
