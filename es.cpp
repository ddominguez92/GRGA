////////////////////////////////////////////////////////////////////
/// File "es.cpp"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <stack>
#include "auxFuncs.h"
#include "constants.h"
#include "es.h"
#include "parameters.h"

using namespace std;
using namespace GRGA;

iESType * EvolutionStrategy::mutate(iESType * p1, float proportion )
{
  auto individual = new iESType;
  individual->formula = p1->formula;
  individual->score = -1;
  individual->parameters = (xreal *)malloc(sizeof(xreal) * p1->formula->length);
  individual->control = (xreal *)malloc(sizeof(xreal) * p1->formula->length);

  int n = p1->formula->n;
  float correctionFactor;
  if (proportion <= 0.2)
    correctionFactor = 10;
  else
    correctionFactor = 1;
  xreal tau_ = correctionFactor * 1.0 / sqrt(2.0  * n); // Global learning rate
  xreal tau = correctionFactor * 1.0 / sqrt(2.0  * sqrt(n)); // Local learning rate

  xreal n01global = dist01(gen);
  xreal n01local;
  xreal minSigma = p["minSigma"].x;

  for (int i = 0; i < p1->formula->length; i++)
    {
      if (p1->formula->expression[i] == kInd)
        {
            n01local = dist01(gen);
            individual->control[i] = max(p1->control[i] * std::exp(tau_ * n01global + tau * n01local), minSigma);
            individual->parameters[i] = p1->parameters[i] + individual->control[i] * n01local;
        }
    }

  return individual;
}

pESType * EvolutionStrategy::initialPopulation(fType *f, int age)
{
  auto pop = new pESType;

  auto individual = new iESType;
  individual->formula = f;
  individual->score = -1;

  individual->parameters = (xreal *)malloc(sizeof(xreal) * f->length);
  individual->control = (xreal *)malloc(sizeof(xreal) * f->length);

  for (int j = 0; j < f->length; j++)
    {
      individual->parameters[j] = f->parameters[j];
      individual->control[j] = p["initialSigma"].x / (std::log2(age) + 1);
    }

  pop->push_back(individual);

  int i;

  xreal initialSigma = p["initialSigma"].x;
  int accurateSizeES = p["accurateSizeES"].i;
  int coarseSizeES = p["coarseSizeES"].i;

  xreal denominator = age + 1;

  for (i = 1; i < p["populationSizeES"].i; i++)
    {
      auto individual = new iESType;
      individual->formula = f;
      individual->score = -1;

      individual->parameters = (xreal *)malloc(sizeof(xreal) * f->length);
      individual->control = (xreal *)malloc(sizeof(xreal) * f->length);

      std::random_device rd;
      std::mt19937 gen(rd());

      xreal sigma = initialSigma;
      if (i <= accurateSizeES)
        sigma = sigma / denominator;
      else if (i <= accurateSizeES + coarseSizeES)
        sigma = sigma * denominator;

      for (int j = 0; j < f->length; j++)
        {
          if (f->expression[j] == kInd)
            {
              std::normal_distribution<> d(f->parameters[j], sigma);
              individual->parameters[j] = d(gen);
              individual->control[j] = sigma;
            }
          else
            {
              individual->parameters[j] = -1;
              individual->control[j] = -1;
            }
        }

      pop->push_back(individual);
    }

  return pop;
}

