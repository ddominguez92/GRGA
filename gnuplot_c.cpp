////////////////////////////////////////////////////////////////////
/// File "gnuplot_c.h"
/// Project GRGA: Generalized Regression based on Genetic Algorithms
/// Original Developer: Numerix-DSP
/// Webpage: http://www.numerix-dsp.com/files/
/// Modified by Daniel Dominguez Catena
////////////////////////////////////////////////////////////////////

// Gnuplot/C interface library
// Please ensure that the system path includes an entry for the gnuplot binary folder

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gnuplot_c.h"

#define GPC_REPLOT 0                // This is being used for testing multiplot/replot which has some strange side effects so is not used in the release

#if defined (_MSC_VER)			        // Defined by Microsoft compilers
  #include <windows.h>
  #if (GPC_DEBUG == 1)
    #define GNUPLOT_CMD "pgnuplot -persist"                     // Window pipe version
    // #define GNUPLOT_CMD "gnuplot -persist"                      // Do not pipe the text output to null so that it can be used for debugging
    // #define GNUPLOT_CMD "gnuplot -persist > debug.log 2>&1"     // Pipe the text output to debug.log for debugging
  #else
    #define GNUPLOT_CMD "gnuplot -persist > /nul 2>&1"          // Pipe the text output to null for higher performance
  #endif
  #define popen _popen
  #define pclose _pclose
  #define sleep Sleep
  // #pragma warning(disable:4996)                               // -D "_CRT_SECURE_NO_WARNINGS=1"
#else 
  #if (GPC_DEBUG == 1)
    #define GNUPLOT_CMD "gnuplot"      // Do not pipe the text output to null so that it can be used for debugging
  #else
    //#define GNUPLOT_CMD "gnuplot -persist > debug.log"          // Pipe the text output to debug.log for debugging
    //#define GNUPLOT_CMD "gnuplot -persist > /dev/null 2>&1"     // Pipe the text output to null for higher performance
    #define GNUPLOT_CMD "gnuplot > /dev/null 2>&1"     // Pipe the text output to null for higher performance
  #endif
#endif

h_GPC_Plot *gpc_init_2d (char *plotTitle,
  char *xLabel,
  char *yLabel,
  double scalingMode,
  enum gpcPlotSignMode signMode,
  enum gpcMultiFastMode multiFastMode,
  enum gpcKeyMode keyMode)

{
	h_GPC_Plot *plotHandle;                                       // Create plot

	plotHandle = (h_GPC_Plot*) malloc (sizeof (h_GPC_Plot));      // Malloc plot and check for error
	if (plotHandle == NULL)
	{
		return (plotHandle);
	}

  plotHandle->pipe = popen (GNUPLOT_CMD, "w");                  // Open pipe to Gnuplot and check for error
  if (plotHandle->pipe == NULL)
  {
    free (plotHandle);
		return (plotHandle);
  }

  strcpy (plotHandle->plotTitle, plotTitle);                    // Set plot title in handle
  plotHandle->multiFastMode = multiFastMode;                    // Set multiplot / fastplot mode in handle

  if (multiFastMode == GPC_MULTIPLOT)
  {
    plotHandle->multiPlot2DFlag = GPC_TRUE;                     // This is a 2D plot - used in gpc_close ()
  }
  else
  {
    plotHandle->multiPlot2DFlag = GPC_FALSE;                    // This is a 2D plot - used in gpc_close ()
  }

  fprintf (plotHandle->pipe, "set term wxt 0 title \"%s\" size %d, %d\n", plotHandle->plotTitle, CANVAS_WIDTH, CANVAS_HEIGHT); // Set the plot
  fprintf (plotHandle->pipe, "set lmargin at screen %4.8lf\n", PLOT_LMARGIN); // Define the margins so that the graph is 512 pixels wide
  fprintf (plotHandle->pipe, "set rmargin at screen %4.8lf\n", PLOT_RMARGIN);
  fprintf (plotHandle->pipe, "set border back\n");              // Set border behind plot

  fprintf (plotHandle->pipe, "set xlabel \"%s\"\n", xLabel);    // Set the X label
  fprintf (plotHandle->pipe, "set ylabel \"%s\"\n", yLabel);    // Set the Y label
  fprintf (plotHandle->pipe, "set grid x y\n");                 // Turn on the grid
  fprintf (plotHandle->pipe, "set tics out nomirror\n");        // Tics format
  fprintf (plotHandle->pipe, "set mxtics 4\n");
  fprintf (plotHandle->pipe, "set mytics 2\n");

  if (keyMode == GPC_KEY_ENABLE)
  {
    fprintf (plotHandle->pipe, "set key out vert nobox\n");     // Legend / key location
  }
  else
  {
    fprintf (plotHandle->pipe, "unset key\n");                  // Disable legend / key
  }

  if (scalingMode == GPC_AUTO_SCALE)                            // Set the Y axis scaling
  {
    fprintf (plotHandle->pipe, "set autoscale  yfix\n");        // Auto-scale Y axis
  }
  else
  {
    if (signMode == GPC_SIGNED)                                // Signed numbers (positive and negative)
    {
      fprintf (plotHandle->pipe, "set yrange [%lf:%lf]\n", -scalingMode, scalingMode);
    }
    else if (signMode == GPC_POSITIVE)                         // 0 to +ve Max
    {
      fprintf (plotHandle->pipe, "set yrange [0.0:%lf]\n", scalingMode);
    }
    else                                                       // GPC_NEGAIVE - -ve Min to 0
    {
      fprintf (plotHandle->pipe, "set yrange [%lf:0.0]\n", -scalingMode);
    }
  }

  fflush (plotHandle->pipe);                                    // flush the pipe

  return (plotHandle);
}

int gpc_plot_2df(h_GPC_Plot *plotHandle,
  float *pData,
  int graphLength,
  char *pDataName,
  double xMin,
  double xMax,
  char *plotType,
  char *pColour,
  enum gpcNewAddGraphMode addMode,
  bool plot)

{
  int   i;
  FILE  *gpdtFile;
  char  tmpFilename [30];
  struct stat   fileStatBuffer;

  if (plotHandle->multiFastMode == GPC_MULTIPLOT)        // GPC_MULTIPLOT
  {
    if (addMode == GPC_NEW)             // GPC_NEW
    {

      // fprintf (plotHandle->pipe, "set autoscale x\n");         // Auto-scale Y axis
      plotHandle->numberOfGraphs = 0;

      i = -1;
      do                                                        // Create a unique local filename - Note this is NOT MT safe !
      {
        i++;
        sprintf (tmpFilename, "%d-0.gpdt", i);
      } while (stat (tmpFilename, &fileStatBuffer) == 0);
      plotHandle->filenameRootId = i;
    }
    else                                // GPC_ADD
    {
      plotHandle->numberOfGraphs++;
      if (plotHandle->numberOfGraphs >= (MAX_NUM_GRAPHS - 1))   // Check we haven't overflowed the maximum number of graphs
      {
        return (GPC_ERROR);
      }
    }

    sprintf (plotHandle->graphArray[plotHandle->numberOfGraphs].filename, "%d-%d.gpdt", plotHandle->filenameRootId, plotHandle->numberOfGraphs);
    sprintf (plotHandle->graphArray[plotHandle->numberOfGraphs].title, "%s", pDataName);
    sprintf (plotHandle->graphArray[plotHandle->numberOfGraphs].formatString, "%s lc rgb \"%s\"", plotType, pColour);

    gpdtFile = fopen (plotHandle->graphArray[plotHandle->numberOfGraphs].filename, "w");    // Open temporary files
    for (i = 0; i < graphLength; i++)                           // Write data to intermediate file
    {
      fprintf (gpdtFile, "%lf %lf\n", xMin + ((((double)i) * (xMax - xMin)) / ((double)(graphLength - 1))), pData[i]);
    }
    fclose (gpdtFile);

    if (plot)
    {
      fprintf (plotHandle->pipe, "plot \"%s\" using 1:2 title \"%s\" with %s", plotHandle->graphArray[0].filename, plotHandle->graphArray[0].title, plotHandle->graphArray[0].formatString);  // Send start of plot and first plot command
      for (i = 1; i <= plotHandle->numberOfGraphs; i++)           // Send individual plot commands
      {
  #if defined (_MSC_VER)			        // Defined by Microsoft compilers
        fprintf (plotHandle->pipe, ", \\\r \"%s\" using 1:2 title \"%s\" with %s", plotHandle->graphArray[i].filename, plotHandle->graphArray[i].title, plotHandle->graphArray[i].formatString);  // Set plot format
  #else
        fprintf (plotHandle->pipe, ", \\\n \"%s\" using 1:2 title \"%s\" with %s", plotHandle->graphArray[i].filename, plotHandle->graphArray[i].title, plotHandle->graphArray[i].formatString);  // Set plot format
  #endif
      }
      fprintf (plotHandle->pipe, "\n");                           // Send end of plot command
    }
  }
  else                                                  // GPC_FASTPLOT
  {
    if (addMode == GPC_NEW)
    {
      fprintf (plotHandle->pipe, "set xrange [%lf:%lf]\n", xMin - (0.5 * ((xMax - xMin) / (graphLength - 1))),
                                                          xMax + (0.5 * ((xMax - xMin) / (graphLength - 1))));  // Set length of X axis
    }

    fprintf (plotHandle->pipe, "plot '-' using 1:2 title \"%s\" with %s lc rgb \"%s\"\n", pDataName, plotType, pColour);  // Set plot format
    for (i = 0; i < graphLength; i++)                           // Copy the data to gnuplot
    {
      fprintf (plotHandle->pipe, "%lf %lf\n", xMin + ((((double)i) * (xMax - xMin)) / ((double)(graphLength - 1))),
                                             pData[i]);
    }
    fprintf (plotHandle->pipe, "e\n");                          // End of dataset
  }                                                  // End of GPC_MULTIPLOT/GPC_FASTPLOT

  fflush (plotHandle->pipe);                                    // Flush the pipe

#if GPC_DEBUG
  sleep (1);                                                  // Slow down output so that pipe doesn't overflow when logging results
#endif

  return (GPC_NO_ERROR);
}

int gpc_plot_2dd(h_GPC_Plot *plotHandle,
  double *pData,
  int graphLength,
  char *pDataName,
  double xMin,
  double xMax,
  char *plotType,
  char *pColour,
  enum gpcNewAddGraphMode addMode,
  bool plot)

{
  int   i;
  FILE  *gpdtFile;
  char  tmpFilename [30];
  struct stat   fileStatBuffer;

  if (plotHandle->multiFastMode == GPC_MULTIPLOT)        // GPC_MULTIPLOT
  {
    if (addMode == GPC_NEW)             // GPC_NEW
    {

      // fprintf (plotHandle->pipe, "set autoscale x\n");         // Auto-scale Y axis
      plotHandle->numberOfGraphs = 0;

      i = -1;
      do                                                        // Create a unique local filename - Note this is NOT MT safe !
      {
        i++;
        sprintf (tmpFilename, "%d-0.gpdt", i);
      } while (stat (tmpFilename, &fileStatBuffer) == 0);
      plotHandle->filenameRootId = i;
    }
    else                                // GPC_ADD
    {
      plotHandle->numberOfGraphs++;
      if (plotHandle->numberOfGraphs >= (MAX_NUM_GRAPHS - 1))   // Check we haven't overflowed the maximum number of graphs
      {
        return (GPC_ERROR);
      }
    }

    sprintf (plotHandle->graphArray[plotHandle->numberOfGraphs].filename, "%d-%d.gpdt", plotHandle->filenameRootId, plotHandle->numberOfGraphs);
    sprintf (plotHandle->graphArray[plotHandle->numberOfGraphs].title, "%s", pDataName);
    sprintf (plotHandle->graphArray[plotHandle->numberOfGraphs].formatString, "%s lc rgb \"%s\"", plotType, pColour);

    gpdtFile = fopen (plotHandle->graphArray[plotHandle->numberOfGraphs].filename, "w");    // Open temporary files
    for (i = 0; i < graphLength; i++)                           // Write data to intermediate file
    {
      fprintf (gpdtFile, "%lf %lf\n", xMin + ((((double)i) * (xMax - xMin)) / ((double)(graphLength - 1))), pData[i]);
    }
    fclose (gpdtFile);

    if (plot)
    {
      fprintf (plotHandle->pipe, "plot \"%s\" using 1:2 title \"%s\" with %s", plotHandle->graphArray[0].filename, plotHandle->graphArray[0].title, plotHandle->graphArray[0].formatString);  // Send start of plot and first plot command
      for (i = 1; i <= plotHandle->numberOfGraphs; i++)           // Send individual plot commands
      {
  #if defined (_MSC_VER)			        // Defined by Microsoft compilers
        fprintf (plotHandle->pipe, ", \\\r \"%s\" using 1:2 title \"%s\" with %s", plotHandle->graphArray[i].filename, plotHandle->graphArray[i].title, plotHandle->graphArray[i].formatString);  // Set plot format
  #else
        fprintf (plotHandle->pipe, ", \\\n \"%s\" using 1:2 title \"%s\" with %s", plotHandle->graphArray[i].filename, plotHandle->graphArray[i].title, plotHandle->graphArray[i].formatString);  // Set plot format
  #endif
      }
      fprintf (plotHandle->pipe, "\n");                           // Send end of plot command
    }
  }
  else                                                  // GPC_FASTPLOT
  {
    if (addMode == GPC_NEW)
    {
      fprintf (plotHandle->pipe, "set xrange [%lf:%lf]\n", xMin - (0.5 * ((xMax - xMin) / (graphLength - 1))),
                                                          xMax + (0.5 * ((xMax - xMin) / (graphLength - 1))));  // Set length of X axis
    }

    fprintf (plotHandle->pipe, "plot '-' using 1:2 title \"%s\" with %s lc rgb \"%s\"\n", pDataName, plotType, pColour);  // Set plot format
    for (i = 0; i < graphLength; i++)                           // Copy the data to gnuplot
    {
      fprintf (plotHandle->pipe, "%lf %lf\n", xMin + ((((double)i) * (xMax - xMin)) / ((double)(graphLength - 1))),
                                             pData[i]);
    }
    fprintf (plotHandle->pipe, "e\n");                          // End of dataset
  }                                                  // End of GPC_MULTIPLOT/GPC_FASTPLOT

  fflush (plotHandle->pipe);                                    // Flush the pipe

#if GPC_DEBUG
  sleep (1);                                                  // Slow down output so that pipe doesn't overflow when logging results
#endif

  return (GPC_NO_ERROR);
}

void gpc_close (h_GPC_Plot *plotHandle)
{
  int i;

  fprintf (plotHandle->pipe, "exit\n");                         // Close GNUPlot
  pclose (plotHandle->pipe);                                    // Close the pipe to Gnuplot
  if (plotHandle->multiPlot2DFlag == GPC_TRUE)                  // If this is a 2D plot we need to delete the temporary files
  {
    for (i = 0; i <= plotHandle->numberOfGraphs; i++)           // Remove all temporary files
    {
      remove (plotHandle->graphArray[i].filename);
    }
  }

  free (plotHandle);                                            // Free the plot
  plotHandle = NULL;

}

