////////////////////////////////////////////////////////////////////
/// File "tournament.cpp"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Author: Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

#include <iostream>
#include <sstream>
#include "direction.h"
#include "evaluation.h"
#include "parameters.h"
#include "plot.h"

using namespace GRGA;

void Plot::initializePlots()
{
  std::string errorMode = "Mean";
  if (p["errorMode"].i == kErrorMax)
    errorMode = "Max";

  std::string errorFunction = "absolute";
  if (p["errorFunction"].i == kEFuncRel)
    errorFunction = "relative";

  std::ostringstream scoreOSS;
  scoreOSS << "Score (" << errorMode << " " << errorFunction << " error)";

  Direction::h2DPlot =
      gpc_init_2d (const_cast<char*>("Evolution"),             // Plot title
                   const_cast<char*>("Time (iterations)"),                  // X-Axis label
                   const_cast<char*>(scoreOSS.str().c_str()),             // Y-Axis label
                   GPC_AUTO_SCALE,          // Scaling mode
                   GPC_SIGNED,              // Sign mode
                   GPC_MULTIPLOT,           // Multiplot / fast plot mode
                   GPC_KEY_ENABLE);         // Legend / key mode
  if (Direction::h2DPlot == NULL)                    // Plot creation failed - e.g is server running ?
    {
      printf ("\nPlot creation failure. Please ensure gnuplot is located on your system path\n");
      exit (-1);
    }
  Evaluation::h2DPlot =                              // Initialize plot
      gpc_init_2d (const_cast<char*>("Results"),             // Plot title
                   const_cast<char*>("t"),                  // X-Axis label
                   const_cast<char*>("value"),             // Y-Axis label
                   GPC_AUTO_SCALE,          // Scaling mode
                   GPC_SIGNED,              // Sign mode
                   GPC_MULTIPLOT,           // Multiplot / fast plot mode
                   GPC_KEY_ENABLE);         // Legend / key mode
  if (Evaluation::h2DPlot == NULL)                    // Plot creation failed - e.g is server running ?
    {
      printf ("\nPlot creation failure. Please ensure gnuplot is located on your system path\n");
      exit (-1);
    }
}

void Plot::closePlots()
{
  if (Direction::h2DPlot != NULL)
    gpc_close(Direction::h2DPlot);
  if (Evaluation::h2DPlot != NULL)
    gpc_close(Evaluation::h2DPlot);

  system ("rm *.gpdt");
}

void Plot::plotIteration(int i, scorereal *scores, scorereal *thresholdBar, scorereal *lengths, iType *bestind)
{
  std::string errorMode = "Mean";
  if (p["errorMode"].i == kErrorMax)
    errorMode = "Max";

  std::string errorFunction = "absolute";
  if (p["errorFunction"].i == kEFuncRel)
    errorFunction = "relative";

  fprintf (Direction::h2DPlot->pipe, "clear\n");
  fprintf (Direction::h2DPlot->pipe, "set title \"%s %s error over time for %s\"\n",
           errorMode.c_str(), errorFunction.c_str(),
           targetData->name.c_str()); // Set the plot title
  fprintf (Direction::h2DPlot->pipe, "set yrange [0:*]\n");
  fprintf (Direction::h2DPlot->pipe, "set y2range [0:%d]\n", targetData->length);
  fprintf (Direction::h2DPlot->pipe, "set y2tics 0, 5, %d textcolor rgb \"orange\"\n",
           targetData->length);
  fprintf (Direction::h2DPlot->pipe, "set y2tics add (\"Data length\" %d)\n", targetData->length);
  gpc_plot_2ds (Direction::h2DPlot,              // Plot handle
                scores,                 // Dataset
                i + 1,         // Number of data points
                const_cast<char*>("Score"),            // Dataset title
                0,                 // Minimum X value
                i,     // Maximum X value
                //"line pt 7 ps 0.5",  // Plot type
                const_cast<char*>("filledcurve y1=0"),
                const_cast<char*>("green"),         // Colour
                GPC_NEW);              // Add plot
  gpc_plot_2ds (Direction::h2DPlot,              // Plot handle
                thresholdBar,                 // Dataset
                2,         // Number of data points
                const_cast<char*>("Error Threshold"),            // Dataset title
                0,                 // Minimum X value
                i,     // Maximum X value
                const_cast<char*>("line pt 7 ps 0.5"),  // Plot type
                const_cast<char*>("red"),         // Colour
                GPC_ADD);              // Add plot
  gpc_plot_2ds (Direction::h2DPlot,              // Plot handle
                lengths,                 // Dataset
                i + 1,         // Number of data points
                const_cast<char*>("Length"),            // Dataset title
                0,                 // Minimum X value
                i,     // Maximum X value
                const_cast<char*>("line pt 7 ps 0.5"),  // Plot type
                const_cast<char*>("orange \" axes x1y2 \""),         // Colour
                GPC_ADD);              // Add plot

  Evaluation::score(bestind->formula, true);
}

void Plot::plotFinalResults(int i, scorereal * scores, scorereal * thresholdBar, scorereal * lengths, iType * bestind)
{
  if (i >= p["numIters"].i)
    i = p["numIters"].i - 1;

  std::string errorMode = "Mean";
  if (p["errorMode"].i == kErrorMax)
    errorMode = "Max";

  std::string errorFunction = "absolute";
  if (p["errorFunction"].i == kEFuncRel)
    errorFunction = "relative";

  std::ostringstream scoreOSS;
  scoreOSS << "Score (" << errorMode << " " << errorFunction << " error)";

  Direction::h2DPlot =
      gpc_init_2d (const_cast<char*>("Evolution"),             // Plot title
                   const_cast<char*>("Time (iterations)"),                  // X-Axis label
                   const_cast<char*>(scoreOSS.str().c_str()),             // Y-Axis label
                   GPC_AUTO_SCALE,          // Scaling mode
                   GPC_SIGNED,              // Sign mode
                   GPC_MULTIPLOT,           // Multiplot / fast plot mode
                   GPC_KEY_ENABLE);         // Legend / key mode
  if (Direction::h2DPlot == NULL)                    // Plot creation failed - e.g is server running ?
    {
      fprintf (stderr, "\nPlot creation failure. Please ensure gnuplot is located on your system path\n");
      exit (1);
    }

  fprintf (Direction::h2DPlot->pipe, "\n");
  fprintf (Direction::h2DPlot->pipe, "set terminal postscript color solid\n"); // Output
  fprintf (Direction::h2DPlot->pipe, "set output \"./results/%s_evolution.ps\"\n",
           targetData->name.c_str());

  fprintf (Direction::h2DPlot->pipe, "set title \"%s %s error over time for %s\"\n",
           errorMode.c_str(), errorFunction.c_str(),
           targetData->name.c_str()); // Set the plot title
  fprintf (Direction::h2DPlot->pipe, "set yrange [0:*]\n");
  fprintf (Direction::h2DPlot->pipe, "set y2range [0:%d]\n", targetData->length);
  fprintf (Direction::h2DPlot->pipe, "set y2tics 0, 5, %d textcolor rgb \"orange\"\n",
           targetData->length);
  fprintf (Direction::h2DPlot->pipe, "set y2tics add (\"Data length\" %d) ", targetData->length);
  gpc_plot_2ds (Direction::h2DPlot,              // Plot handle
                scores,                 // Dataset
                i + 1,         // Number of data points
                const_cast<char*>("Score"),            // Dataset title
                0,                 // Minimum X value
                i,     // Maximum X value
                //"line pt 7 ps 0.5",  // Plot type
                const_cast<char*>("filledcurve y1=0"),
                const_cast<char*>("green"),         // Colour
                GPC_NEW,              // Add plot
                false);               // Do not plot yet
  gpc_plot_2ds (Direction::h2DPlot,              // Plot handle
                thresholdBar,                 // Dataset
                2,         // Number of data points
                const_cast<char*>("Error Threshold"),            // Dataset title
                0,                 // Minimum X value
                i,     // Maximum X value
                const_cast<char*>("line pt 7 ps 0.5"),  // Plot type
                const_cast<char*>("red"),         // Colour
                GPC_ADD,              // Add plot
                true);                // Do not plot yet
  gpc_plot_2ds (Direction::h2DPlot,              // Plot handle
                lengths,                 // Dataset
                i + 1,         // Number of data points
                const_cast<char*>("Length"),            // Dataset title
                0,                 // Minimum X value
                i,     // Maximum X value
                const_cast<char*>("line pt 7 ps 0.5"),  // Plot type
                const_cast<char*>("orange\" axes x1y2 \""),         // Colour
                GPC_ADD,               // Add plot
                true);               // Now you can plot

  if (Direction::h2DPlot != NULL)
    gpc_close(Direction::h2DPlot);

  Evaluation::h2DPlot =                              // Initialize plot
      gpc_init_2d (const_cast<char*>("Results"),             // Plot title
                   const_cast<char*>("t"),                  // X-Axis label
                   const_cast<char*>("Value"),             // Y-Axis label
                   GPC_AUTO_SCALE,          // Scaling mode
                   GPC_SIGNED,              // Sign mode
                   GPC_MULTIPLOT,           // Multiplot / fast plot mode
                   GPC_KEY_ENABLE);         // Legend / key mode
  if (Evaluation::h2DPlot == NULL)                    // Plot creation failed - e.g is server running ?
    {
      printf ("\nPlot creation failure. Please ensure gnuplot is located on your system path\n");
      exit (1);
    }

  fprintf (Evaluation::h2DPlot->pipe, "\n");
  fprintf (Evaluation::h2DPlot->pipe, "set terminal postscript color solid\n"); // Set the plot
  fprintf (Evaluation::h2DPlot->pipe, "set output \"./results/%s_result.ps\"\n", targetData->name.c_str());
  fprintf (Evaluation::h2DPlot->pipe, "set title \"Function evaluation for %s\"\n",
           targetData->name.c_str()); // Set the plot title
  Evaluation::score(bestind->formula, true);

  if (Evaluation::h2DPlot != NULL)
    gpc_close(Evaluation::h2DPlot);

  system ("rm *.gpdt");
}

void Plot::plotEvaluation(xreal * results)
{
  if (GRGA_PLOT_RESULTS == 1 )
    {
      if (Evaluation::h2DPlot == NULL)
        {
          Evaluation::h2DPlot =                              // Initialize plot
            gpc_init_2d (const_cast<char*>("Results"),             // Plot title
                         const_cast<char*>("t"),                  // X-Axis label
                         const_cast<char*>("Value"),             // Y-Axis label
                         GPC_AUTO_SCALE,          // Scaling mode
                         GPC_SIGNED,              // Sign mode
                         GPC_MULTIPLOT,           // Multiplot / fast plot mode
                         GPC_KEY_ENABLE);         // Legend / key mode
          if (Evaluation::h2DPlot == NULL)                    // Plot creation failed - e.g is server running ?
           {
             printf ("\nPlot creation failure. Please ensure gnuplot is located on your system path\n");
             exit (1);
           }
        }

      fprintf (Evaluation::h2DPlot->pipe, "clear\n");
    }

  gpc_plot_2dx (Evaluation::h2DPlot,              // Plot handle
               targetData->data,                 // Dataset
               p["currentTargetLength"].i,         // Number of data points
               const_cast<char*>("Expected"),            // Dataset title
               1,                 // Minimum X value
               p["currentTargetLength"].i,     // Maximum X value
               const_cast<char*>("line pt 7 ps 0.5"),  // Plot type
               const_cast<char*>("blue"),         // Colour
               GPC_NEW, false);              // Add plot

  gpc_plot_2dx (Evaluation::h2DPlot,              // Plot handle
               results,                 // Dataset
               p["currentTargetLength"].i - p["maxPrev"].i,         // Number of data points
               const_cast<char*>("Actual"),            // Dataset title
               p["maxPrev"].i + 1,                 // Minimum X value
               p["currentTargetLength"].i,     // Maximum X value
               const_cast<char*>("line pt 7 ps 0.5"),  // Plot type
               const_cast<char*>("light-red"),         // Colour
               GPC_ADD);              // Add plot

}
