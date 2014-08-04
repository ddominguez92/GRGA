////////////////////////////////////////////////////////////////////
/// File "constants.h"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#ifndef GRGA_CONSTANTS_H
#define GRGA_CONSTANTS_H

#include <map>

namespace GRGA
{

  enum eSymbol {

    //Binary Operators
    kOperatorsBin = 0,
    kAdd,
    kSub,
    kMul,
    kDiv,
    kLog,
    kPow,
    kComp,
    kMod,
    kOperatorsBinEnd,

    //Operands
    kOperands = 10,
    kTime,
    kPrev,
    kInd,
    k1,
    k0,
    kneg1,
    kOperandsEnd,

    //Meta
    kEnd = 20
  };

  const std::map<eSymbol, int> mParams = {
    {kAdd, 1},
    {kSub, 1},
    {kMul, 1},
    {kDiv, 1},
    {kPow, 1},
    {kLog, 1},
    {kComp, 1},
    {kMod, 1},
    {kTime, -1},
    {kPrev, -1},
    {kInd, -1},
    {k1, -1},
    {k0, -1},
    {kneg1, -1},
    {kEnd, 1}
  };

  const std::map<eSymbol, std::string> mStrings = {
    {kAdd, "+"},
    {kSub, "-"},
    {kMul, "*"},
    {kDiv, "/"},
    {kPow, "^"},
    {kLog, "log"},
    {kComp, ">"},
    {kMod, "%"},
    {kTime, "T"},
    {kPrev, "P"},
    {kInd, "K"},
    {k1, "1"},
    {k0, "0"},
    {kneg1, "(-1)"},
    {kEnd, "#"}
  };

  enum eEvalMode {
    kEvalSinglePredict,
    kEvalFullSeriesPredict
  };

  enum eErrorMode {
    kErrorMean,
    kErrorMax
  };

  enum eErrorFunction {
    kEFuncRel,
    kEFuncAbs
  };

  const std::map<std::string, int> cStrings = {
    {"kEvalSinglePredict", kEvalSinglePredict},
    {"kEvalFullSeriesPredict", kEvalFullSeriesPredict},
    {"kErrorMean", kErrorMean},
    {"kErrorMax", kErrorMax},
    {"kEFuncRel", kEFuncRel},
    {"kEFuncAbs", kEFuncAbs},
  };
}

#endif // GRGA_CONSTANTS_H
