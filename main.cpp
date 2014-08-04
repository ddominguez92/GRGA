////////////////////////////////////////////////////////////////////
/// File "main.cpp"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "auxFuncs.h"
#include "direction.h"
#include "omp.h"
#include "parameters.h"
#include "types.h"

using namespace std;
using namespace GRGA;

typedef struct inputData {
  std::vector<xreal> data;
  std::string name;
} inputData;

int main()
{

  initRand();

  ifstream infile("./input/tasks.txt");
  string line;

  if (!infile.is_open())
    {
      cout << "Error reading task file" << endl;
      exit(-1);
    }

  getline(infile, line);

  {
    istringstream iss(line);
    string output;

    iss.ignore(50, ':');
    if (iss.peek() == ' ')
      iss.ignore(1, ' ');

    iss >> output;

    if (output.compare("screen") == 0)
      {
        cout << "Full output on screen" << endl << endl;
        GRGA_PLOT_RESULTS = 1;
        GRGA_PLOT_RESULTS_TO_FILE = 0;
        LOG_TO_FILE = 0;
        OMIT_LOG = 0;
      }
    else if (output.compare("file") == 0)
      {
        cout << "Full output on files" << endl << endl;
        GRGA_PLOT_RESULTS = 0;
        GRGA_PLOT_RESULTS_TO_FILE = 1;
        LOG_TO_FILE = 1;
        OMIT_LOG = 0;
      }
    else if (output.compare("text") == 0)
      {
        cout << "Text output on screen" << endl << endl;
        GRGA_PLOT_RESULTS = 0;
        GRGA_PLOT_RESULTS_TO_FILE = 0;
        LOG_TO_FILE = 0;
        OMIT_LOG = 0;
      }
    else if (output.compare("none") == 0)
      {
        cout << "No output" << endl << endl;
        GRGA_PLOT_RESULTS = 0;
        GRGA_PLOT_RESULTS_TO_FILE = 0;
        LOG_TO_FILE = 0;
        OMIT_LOG = 1;
      }
    else
      {
        cout << "Unrecognised output type" << endl << endl;
        exit(-1);
      }
  }

  while (getline(infile, line) && (line.compare(0, 5, "begin") != 0)) {}

  if (line.compare(0, 5, "begin") != 0)
    {
      cout << "Error reading task file" << endl;
      cout << "No begin found on task file" << endl;
      exit(-1);
    }

  while (getline(infile, line) &&
         ((line.length() == 0) ||
         (line.front() == '%'))) {}

  while (line.compare(0, 3, "end") != 0)
    {
      // Read task
      istringstream iss(line);
      string name;
      std::vector<xreal> data;

      iss.ignore(50, ':');
      if (iss.peek() == ' ')
        iss.ignore(1, ' ');

      iss >> name;

      while (getline(infile, line) &&
             ((line.length() == 0) ||
             (line.front() == '%'))) {}

      iss.str(line);
      iss.clear();

      iss.ignore(50, ':');

      xreal tmp;

      while (iss >> tmp)
        {
          data.push_back(tmp);

          if (iss.peek() == ',')
            iss.ignore(1, ',');
          iss >> ws;
        }


      while (getline(infile, line) &&
             ((line.length() == 0) ||
             (line.front() == '%'))) {}

      iss.str(line);
      iss.clear();
      iss.ignore(50, ':');
      string parametersFile;
      iss >> parametersFile;

      parametersFile = "./input/" + parametersFile;

      cout << "Round begin: " << endl;
      cout << "Target data: " << name << endl;

      // Load data
      dType * dS = new dType();
      dS->data = &(data[0]);
      dS->length = data.size();
      dS->name = name;
      targetData = dS;

      Parameters::loadParametersFromFile(parametersFile);

      if (LOG_TO_FILE == 1)
        {
          //Redirect output
          std::string fileName = "./results/" + targetData->name + "_log.txt";
          freopen (fileName.c_str(), "w", stdout);
        }

      // Run
      double start = omp_get_wtime();

      Direction::run();
      double stop = omp_get_wtime();

      if (LOG_TO_FILE == 1)
        {
          cout << "Ran in " << stop - start << " seconds" << endl;
          cout << "for target data: " << name << endl << endl;

          // Restore output
          fflush(stdout);
          freopen ("/dev/tty", "a", stdout);
        }

      cout << "Ran in " << stop - start << " seconds" << endl;
      cout << "for target data: " << name << endl << endl;

      while (getline(infile, line) &&
             ((line.length() == 0) ||
             (line.front() == '%'))) {}

    }


  return 0;

}

