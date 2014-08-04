////////////////////////////////////////////////////////////////////
/// File "parameters.cpp"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <sstream>
#include "parameters.h"

using namespace GRGA;
using namespace std;

int GRGA::GRGA_PLOT_RESULTS = 0;
int GRGA::GRGA_PLOT_RESULTS_TO_FILE = 0;

int GRGA::LOG_TO_FILE = 0;
int GRGA::OMIT_LOG = 0;

map<string, parameter> GRGA::p;

void Parameters::loadParametersFromFile(string fileName)
{
  ifstream infile(fileName);
  string line;

  if (!infile.is_open())
    {
      cout << "Error opening parameter file ";
      cout << "[" << fileName << "]" << endl;
    }

  while (getline(infile, line))
    {
      istringstream iss(line);
      string name;
      char type;
      xreal x;
      scorereal s;
      float f;
      int i;
      string e;

      getline(iss, name, ',');

      if ((name.front() == '%') || (name.length() >= line.length()))
        continue;

      if (iss.peek() == ',')
          iss.ignore(1, ',');
      if (iss.peek() == ' ')
          iss.ignore(1, ' ');
      iss >> type;

      if (iss.peek() == ',')
          iss.ignore(1, ',');
      if (iss.peek() == ' ')
          iss.ignore(1, ' ');

      switch (type)
        {
        case 'i':
          iss >> i;
          p[name].i = i;
          break;
        case 'x':
          iss >> x;
          p[name].x = x;
          break;
        case 'f':
          iss >> f;
          p[name].f = f;
          break;
        case 's':
          iss >> s;
          p[name].s = s;
          break;
        case 'e':
          iss >> e;
          i = cStrings.at(e);
          p[name].i = i;
          break;
        default:
          cout << "Unrecognized type error: " << type << endl;
          exit(-1);
        }
    }

    generateDerivedParameters();
}

void Parameters::generateDerivedParameters()
{
  p["maxFLength"].i = 2 << p["maxDepth"].i;
  p["initialTargetLength"].i = min(p["initialTargetLength"].i,
      targetData->length);
  p["currentTargetLength"].i = p["initialTargetLength"].i;

  p["foreignersSize"].i = p["populationSize"].i *
      p["foreignersPercentage"].f;          // New individuals
  p["mutationSize"].i = p["populationSize"].i *
      p["mutationPercentage"].f;            // Mutated individuals
  p["crossoverSize"].i = p["populationSize"].i *
      p["crossoverPercentage"].f;           // Breeded individuals
  p["elitistReplicationSize"].i = p["populationSize"].i *
      p["elitistReplicationPercentage"].f;
}

dType * GRGA::targetData = NULL;
