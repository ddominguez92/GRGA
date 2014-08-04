////////////////////////////////////////////////////////////////////
/// File "direction.cpp"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#include <cmath>
#include <csignal>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include "auxFuncs.h"
#include "constants.h"
#include "direction.h"
#include "es.h"
#include "evaluation.h"
#include "generation.h"
#include "gp.h"
#include "omp.h"
#include "parameters.h"
#include "plot.h"
#include "tournament.h"

using namespace GRGA;

scorereal Direction::run()
{

  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = interruptHandler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);

  scorereal scores[p["numIters"].i];
  scorereal lengths[p["numIters"].i];
  scorereal thresholdBar[] = { p["finishErrorThreshold"].s, p["finishErrorThreshold"].s };

  if (GRGA_PLOT_RESULTS == 1)
    {
      Plot::initializePlots();
    }

  auto population = GeneticProgramming::initialPopulation();

  for(auto it = population->begin(); it != population->end(); it++)
    {
      (* it)->score = Evaluation::score((* it)->formula);
    }

  sortPType(population);

  int stepUpCounter = 0;

  int i;
  for (i = 0; i < p["numIters"].i; i++)
    {
      // The population begins the iteration already ordered,
      // worst to best.

      population = deriveNextGeneration(population);

      #pragma omp parallel for num_threads(omp_get_num_procs()) shared(population)
      for(int e = 0; e < p["populationSize"].i; e++)
        {
          iType * originalElem = population->at(e);
          if ((i % p["GPESItersRatio"].i == 0) || (i == p["numIters"].i - 1))
            {
              iType * newElem = runES(originalElem);
              if (newElem != originalElem)
                {
                  clearIType(originalElem);
                  population->at(e) = newElem;
                }
              originalElem = population->at(e);
            }
          if (originalElem->score == -1)
            {
              originalElem->score = Evaluation::score(originalElem->formula);
            }
          originalElem->age++;
        }

      sortPType(population);

      auto bestind = population->back();

      if (!OMIT_LOG)
        {
          cout << "Best individual [it " << i << "] for dataset \"" << targetData->name << "\": " << endl;
          printFormula(bestind->formula);
          cout << "Score: " << bestind->score << endl;
          cout << "Age: " << bestind->age << " ESCycles: " << bestind->ESCycles <<
                  " ESCoolDown: " << bestind->ESCoolDown << endl;
          cout << "Current lenght = " << p["currentTargetLength"].i << "/" << targetData->length << endl << endl;

        }

      scores[i] = bestind->score;
      lengths[i] = p["currentTargetLength"].i;


      if (GRGA_PLOT_RESULTS == 1)
        {
          if (i % 10 == 0 || i == p["numIters"].i - 1 ||
              bestind->score < p["finishErrorThreshold"].s)
            {
              Plot::plotIteration(i, scores, thresholdBar, lengths, bestind);
            }

        }

      stepUpCounter++;
      if ((stepUpCounter > p["stepUpLag"].i) && (bestind->score <= p["errorThreshold"].s))
        {
          if (p["currentTargetLength"].i < targetData->length)
            {
              cout << "That was enough! Step it up a bit!" << endl;
              p["currentTargetLength"].i++;
              cout << "Current lenght = " << p["currentTargetLength"].i <<
                      "/" << targetData->length << endl << endl;
              stepUpCounter = 0;

              #pragma omp parallel for num_threads(omp_get_num_procs()) shared(population)
              for(int e = 0; e < p["populationSize"].i; e++)
                {
                  iType * elem = population->at(e);
                  elem->age = 0;
                  elem->ESCoolDown = 0;
                  elem->ESCycles = 0;
                  elem->score = Evaluation::score(elem->formula);
                }
            }
          else
            {
              if (bestind->score < p["finishErrorThreshold"].s)
                {
                  cout << "Looks like we have a winner!" << endl;
                  break;
                }
            }
        }

    }

  auto bestind = population->back();

  if (!OMIT_LOG)
    {
      cout << "Final best individual: " << endl;
      printFormula(bestind->formula);
      cout << "Score: " << bestind->score << " evaluated over a length of " <<
            p["currentTargetLength"].i << " out of " << targetData->length << endl << endl;
    }

  scorereal finalDeviation = bestind->score;

  if(GRGA_PLOT_RESULTS_TO_FILE == 1)
    {
      Plot::plotFinalResults(i, scores, thresholdBar, lengths, bestind);
    }


  clearPType(population);

  if (GRGA_PLOT_RESULTS == 1 )
    {
      printf ("\nHit <CR> to continue ....\n"); getchar (); // Wait for <CR>
      Plot::closePlots();
    }

  return finalDeviation;
}

pType * Direction::deriveNextGeneration(pType * population)
{
  auto newPopulation = new pType;
  newPopulation->reserve(p["populationSize"].i);

  int e = 0;

  for (e = 0; e < p["foreignersSize"].i; e++)
    {
      auto childf = Generation::generate();

      auto child = new iType;
      child->formula = childf;
      child->score = -1;

      newPopulation->push_back(child);
    }

  for (e = 0; e < p["crossoverSize"].i; e++)
    {
      auto parenti1 = Tournament::tournament(population);
      auto parenti2 = Tournament::tournament(population);
      auto parentf1 = population->at(parenti1)->formula;
      auto parentf2 = population->at(parenti2)->formula;
      auto childf = GeneticProgramming::crossover(parentf1, parentf2);
      auto child = new iType;
      child->formula = childf;
      child->score = -1;

      newPopulation->push_back(child);
    }

  for (e = 0; e < p["mutationSize"].i; e++)
    {
      auto parenti1 = Tournament::tournament(population);
      auto parentf1 = population->at(parenti1)->formula;
      auto childf = GeneticProgramming::mutate(parentf1);

      auto child = new iType;
      child->formula = childf;
      child->score = -1;

      newPopulation->push_back(child);
    }

  for (e = 0; e < p["elitistReplicationSize"].i; e++)
    {
      newPopulation->push_back(population->back());
      population->pop_back();
    }

  for (e = 0; (int) newPopulation->size() < p["populationSize"].i; e++)
    {
      int childIndex = RANDOM(0, (int) (population->size() - 1));

      newPopulation->push_back(population->at(childIndex));

      population->at(childIndex) = population->back();
      population->pop_back();
    }

  clearPType(population);
  return newPopulation;
}

iType * Direction::runES(iType * i)
{
  if (i->ESCycles > p["limitCyclesES"].i && i->ESCoolDown < p["coolDownCyclesES"].i)
    {
      i->ESCoolDown++;
      if (i->ESCoolDown >= p["coolDownCyclesES"].i)
        i->ESCycles = 0;
      return i;
    }
  i->ESCycles++;
  i->ESCoolDown = 0;

  unsigned int * cur = i->formula->expression;

  if (i->formula->n == -1)
    {
      i->formula->n = 0;

      while (*cur != kEnd)
        {
          if (*cur == kInd)
            {
              i->formula->n += 1;
            }
          cur++;
        }
    }

  if (i->formula->n == 0)
    {
      return i;
    }

  pESType * population = EvolutionStrategy::initialPopulation(i->formula, i->age);
  pESType::iterator it;

  for ( it = population->begin(); it != population->end(); ++it )
    {
      (*it)->score = Evaluation::score((*it));
    }
  population->sort(compareIESType);

  float lastSuccessfulMutationProportion = 1.0/5.0;

  for (int i = 0; i < p["numItersES"].i; i++)
    {
      pESType * children = new pESType();
      it = population->begin();

      int successfulMutations = 0;

      while ((it != population->end()) && ((int) children->size() < p["offspringES"].i))
        {
          auto child = EvolutionStrategy::mutate(* it, lastSuccessfulMutationProportion);
          child->score = Evaluation::score(child);
          children->push_back(child);

          if (child->score < (* it)->score)
            {
              successfulMutations++;
            }
        }

      lastSuccessfulMutationProportion = (float) successfulMutations / p["offspringES"].i;

      children->sort(compareIESType);

      population->merge((* children), compareIESType);

      while ((int) population->size() > p["populationSizeES"].i)
        {
          clearIESType( population->back() );
          population->pop_back();
        }

      delete children;

    }

  while( population->size() > 1 )
    {
      clearIESType( population->back() );
      population->pop_back();
    }

  iESType * best = population->front();
  delete population;

  fType * fS = new fType();
  fS->expression = (unsigned int *)malloc(sizeof(unsigned int) * p["maxFLength"].i);
  fS->prev = (unsigned int *)malloc(sizeof(unsigned int) * p["maxFLength"].i);;
  fS->parameters = best->parameters;
  fS->length = best->formula->length;
  fS->n = best->formula->n;
  fS->maxLength = p["maxFLength"].i;

  for (int i = 0; i < fS->length; i++)
    {
      fS->expression[i] = best->formula->expression[i];
      fS->prev[i] = best->formula->prev[i];
    }

  iType * result = new iType();
  result->formula = fS;
  result->score = best->score;
  result->age = i->age;
  result->ESCoolDown = i->ESCoolDown;
  result->ESCycles = i->ESCycles;

  free(best->control);
  delete best;

  return result;
}

void Direction::interruptHandler (int s)
{
  printf("Caught signal %d\n",s);

  if (GRGA_PLOT_RESULTS == 1)
    {
      printf ("\nHit <CR> to continue ....\n"); getchar (); // Wait for <CR>
    }
  if (GRGA_PLOT_RESULTS == 1 || GRGA_PLOT_RESULTS_TO_FILE == 1)
    {
      Plot::closePlots();
    }

  exit(1);
}
