////////////////////////////////////////////////////////////////////
/// File "types.h"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#ifndef GRGA_TYPES_H
#define GRGA_TYPES_H

#include <list>
#include <vector>

namespace GRGA
{

  #ifndef XREAL
  #define XREAL
  //typedef float xreal;
  //#define gpc_plot_2dx gpc_plot_2df
  typedef double xreal;
  #define gpc_plot_2dx gpc_plot_2dd
  #endif

  #ifndef SCOREREAL
  #define SCOREREAL
  //typedef float scorereal;
  //#define gpc_plot_2ds gpc_plot_2df
  typedef double scorereal;
  #define gpc_plot_2ds gpc_plot_2dd
  #endif

  //Formula
  typedef struct
  {
    unsigned int * expression;
    unsigned int * prev;
    xreal * parameters;
    int length;
    int n = -1;
    int maxLength;
  } fType;

  //GP Individual
  typedef struct
  {
    fType * formula;
    scorereal score = -1;
    int age = 1;
    int ESCycles = 0;
    int ESCoolDown = 0;
  } iType;

  //GP Population
  typedef std::vector<iType *> pType;


  //ES Individual
  typedef struct
  {
    fType * formula;
    xreal * parameters;
    xreal * control;
    scorereal score = -1;
  } iESType;

  //ES Population
  typedef std::list<iESType *> pESType;

  //Data
  typedef struct
  {
    xreal * data;
    int length;
    std::string name;
  } dType;

}
#endif // GRGA_TYPES_H
