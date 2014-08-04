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
  enum gpcNewAddGraphMode addMode)

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
  enum gpcNewAddGraphMode addMode)

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


h_GPC_Plot *gpc_init_xy (char *plotTitle,
  char *xLabel,
  char *yLabel,
  double dimension,
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

  plotHandle->multiPlot2DFlag = GPC_FALSE;                      // This is NOT a 2D plot - used in gpc_close ()
  plotHandle->scalingMode = dimension;                          // Set dimension in handle
  strcpy (plotHandle->plotTitle, plotTitle);                    // Set plot title in handle

  fprintf (plotHandle->pipe, "set term wxt 0 title \"%s\" size %d, %d\n", plotHandle->plotTitle, CANVAS_WIDTH, CANVAS_HEIGHT); // Set the plot
  fprintf (plotHandle->pipe, "set lmargin at screen %4.8lf\n", PLOT_LMARGIN); // Define the margins so that the graph is 512 pixels wide
  fprintf (plotHandle->pipe, "set rmargin at screen %4.8lf\n", PLOT_RMARGIN);
  fprintf (plotHandle->pipe, "set border back\n");              // Set border behind plot

  fprintf (plotHandle->pipe, "set ylabel \"%s\"\n", xLabel);    // Axis labels look better when rotated
  fprintf (plotHandle->pipe, "set xlabel \"%s\"\n", yLabel);
  fprintf (plotHandle->pipe, "set nokey\n");

  fprintf (plotHandle->pipe, "unset border\n");
  fprintf (plotHandle->pipe, "set xtics axis nomirror\n");
  fprintf (plotHandle->pipe, "set ytics axis nomirror\n");
  fprintf (plotHandle->pipe, "unset rtics\n");
  fprintf (plotHandle->pipe, "set zeroaxis\n");

  if (keyMode == GPC_KEY_ENABLE)
  {
    fprintf (plotHandle->pipe, "set key out vert nobox\n");     // Legend / key location
  }
  else
  {
    fprintf (plotHandle->pipe, "unset key\n");                  // Disable legend / key
  }

//  fprintf (plotHandle->pipe, "set clip\n");                     // Enable graph clipping

  fflush (plotHandle->pipe);                                    // flush the pipe

  return (plotHandle);
}


int gpc_plot_xy (h_GPC_Plot *plotHandle,
  ComplexRect_s *pData,
  int graphLength,
  char *pDataName,
  char *plotType,
  char *pColour,
  enum gpcNewAddGraphMode addMode)

{
  int i;

  if (addMode == GPC_NEW)
  {
    if (plotHandle->scalingMode == GPC_AUTO_SCALE)              // Set the X and Y axis scaling
    {
      fprintf (plotHandle->pipe, "set autoscale xy\n");         // Auto-scale Y axis
    }
    else
    {
      fprintf (plotHandle->pipe, "set xrange[-%lf:%lf]\n", plotHandle->scalingMode, plotHandle->scalingMode);
      fprintf (plotHandle->pipe, "set yrange[-%lf:%lf]\n", plotHandle->scalingMode, plotHandle->scalingMode);
    }

    if (plotHandle->numberOfGraphs != 0)
    {
      fprintf (plotHandle->pipe, "unset multiplot\n");          // If there is an existing multiplot then close it
    }
    fprintf (plotHandle->pipe, "set multiplot\n");

    plotHandle->numberOfGraphs = 1;                             // Reset the number of plots
  }
  else                              // GPC_ADD
  {
    plotHandle->numberOfGraphs++;                               // Increment the number of graphs
  }

  fprintf (plotHandle->pipe, "plot '-' title \"%s\" with %s lc rgb \"%s\"\n", pDataName, plotType, pColour);  // Set plot format
  for (i = 0; i < graphLength; i++)                             // Copy the data to gnuplot
  {
    fprintf (plotHandle->pipe, "%lf %lf\n", pData[i].real, pData[i].imag);
  }

  fprintf (plotHandle->pipe, "e\n");                            // End of dataset

  fflush (plotHandle->pipe);                                    // Flush the pipe

#if GPC_DEBUG
  sleep (1);                                                  // Slow down output so that pipe doesn't overflow when logging results
#endif

  return (GPC_NO_ERROR);
}

h_GPC_Plot *gpc_init_pz (char *plotTitle,
  double dimension,
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

  plotHandle->multiPlot2DFlag = GPC_FALSE;                      // This is NOT a 2D plot - used in gpc_close ()
  plotHandle->scalingMode = dimension;                          // Set dimension in handle
  strcpy (plotHandle->plotTitle, plotTitle);                    // Set plot title in handle

  fprintf (plotHandle->pipe, "set term wxt 0 title \"%s\" size %d, %d\n", plotHandle->plotTitle, CANVAS_WIDTH, CANVAS_HEIGHT); // Set the plot
  fprintf (plotHandle->pipe, "set lmargin at screen %4.8lf\n", PLOT_LMARGIN); // Define the margins so that the graph is 512 pixels wide
  fprintf (plotHandle->pipe, "set rmargin at screen %4.8lf\n", PLOT_RMARGIN);
  fprintf (plotHandle->pipe, "set border back\n");              // Set border behind plot

  fprintf (plotHandle->pipe, "set size square\n");
  fprintf (plotHandle->pipe, "set tics scale 0.75\n");

  fprintf (plotHandle->pipe, "set ylabel \"Real\"\n");
  fprintf (plotHandle->pipe, "set xlabel \"Imaginary\"\n");
  fprintf (plotHandle->pipe, "set nokey\n");

  fprintf (plotHandle->pipe, "unset border\n");
  fprintf (plotHandle->pipe, "set xtics axis nomirror\n");
  fprintf (plotHandle->pipe, "set ytics axis nomirror\n");
  fprintf (plotHandle->pipe, "unset rtics\n");
  fprintf (plotHandle->pipe, "set zeroaxis\n");

                                                                // Define line styles for Poles, Zeros and unit circle
  fprintf (plotHandle->pipe, "set style line 1 lc rgb \"#ff0000\" lt 1 lw 1   pt 2 ps 1.5\n");   // Poles - Red
  fprintf (plotHandle->pipe, "set style line 2 lc rgb \"#0000ff\" lt 1 lw 1   pt 6 ps 1.5\n");   // Zeros - Blue
  fprintf (plotHandle->pipe, "set style line 3 lc rgb \"#000000\" lt 1 lw 0.5 pt 5 ps 1.5\n");   // Unit circle - Black

  if (keyMode == GPC_KEY_ENABLE)
  {
    fprintf (plotHandle->pipe, "set key out vert nobox\n");     // Legend / key location
  }
  else
  {
    fprintf (plotHandle->pipe, "unset key\n");                  // Disable legend / key
  }

  fflush (plotHandle->pipe);                                    // flush the pipe

  return (plotHandle);
}


int gpc_plot_pz (h_GPC_Plot *plotHandle,
  ComplexRect_s *pData,
  int graphLength,
  char *pDataName,
  enum gpcPoleZeroMode poleZeroMode,
  enum gpcNewAddGraphMode addMode)

{
  int i;

  if (addMode == GPC_NEW)
  {
    if (plotHandle->scalingMode == GPC_AUTO_SCALE)              // Set the X and Y axis scaling
    {
      fprintf (plotHandle->pipe, "set xrange[-1.5:1.5]\n");
      fprintf (plotHandle->pipe, "set yrange[-1.5:1.5]\n");
    }
    else
    {
      fprintf (plotHandle->pipe, "set xrange[-%lf:%lf]\n", plotHandle->scalingMode, plotHandle->scalingMode);
      fprintf (plotHandle->pipe, "set yrange[-%lf:%lf]\n", plotHandle->scalingMode, plotHandle->scalingMode);
    }

    if (plotHandle->numberOfGraphs != 0)
    {
      fprintf (plotHandle->pipe, "unset multiplot\n");          // If there is an existing multiplot then close it
    }
    fprintf (plotHandle->pipe, "set multiplot\n");

    plotHandle->numberOfGraphs = 1;                             // Reset the number of plots
  }
  else                              // GPC_ADD
  {
    plotHandle->numberOfGraphs++;                               // Increment the number of graphs
  }

  fprintf (plotHandle->pipe, "set parametric\n");               // Plot unit circle
  fprintf (plotHandle->pipe, "set angle degree\n");
  fprintf (plotHandle->pipe, "set trange [0:360]\n");
  fprintf (plotHandle->pipe, "set size square\n");
  fprintf (plotHandle->pipe, "unset parametric\n");

  switch (poleZeroMode)
  {
    case GPC_COMPLEX_POLE :
      fprintf (plotHandle->pipe, "plot '-' title \"%s\" with points ls 1\n", pDataName);  // Set plot format
      for (i = 0; i < graphLength; i++)                         // Copy the data to gnuplot
      {
        fprintf (plotHandle->pipe, "%lf %lf\n", pData[i].real, pData[i].imag);
      }
      break;
    case GPC_CONJUGATE_POLE :
      fprintf (plotHandle->pipe, "plot '-' title \"%s\" with points ls 1\n", pDataName);  // Set plot format
      for (i = 0; i < graphLength; i++)                         // Copy the data to gnuplot
      {
        fprintf (plotHandle->pipe, "%lf %lf\n", pData[i].real, pData[i].imag);
      }
      for (i = 0; i < graphLength; i++)                         // Copy the data to gnuplot - conjugate pole
      {
        fprintf (plotHandle->pipe, "%lf %lf\n", pData[i].real, -pData[i].imag);
      }
      break;
    case GPC_COMPLEX_ZERO :
      fprintf (plotHandle->pipe, "plot '-' title \"%s\" with points ls 2\n", pDataName);  // Set plot format
      for (i = 0; i < graphLength; i++)                         // Copy the data to gnuplot
      {
        fprintf (plotHandle->pipe, "%lf %lf\n", pData[i].real, pData[i].imag);
      }
      break;
    case GPC_CONJUGATE_ZERO :
      fprintf (plotHandle->pipe, "plot '-' title \"%s\" with points ls 2\n", pDataName);  // Set plot format
      for (i = 0; i < graphLength; i++)                         // Copy the data to gnuplot
      {
        fprintf (plotHandle->pipe, "%lf %lf\n", pData[i].real, pData[i].imag);
      }
      for (i = 0; i < graphLength; i++)                         // Copy the data to gnuplot - conjugate zero
      {
        fprintf (plotHandle->pipe, "%lf %lf\n", pData[i].real, -pData[i].imag);
      }
      break;
  }

  fprintf (plotHandle->pipe, "e\n");                            // End of dataset

  fflush (plotHandle->pipe);                                    // Flush the pipe

#if GPC_DEBUG
  sleep (1);                                                  // Slow down output so that pipe doesn't overflow when logging results
#endif

  return (GPC_NO_ERROR);
}

h_GPC_Plot *gpc_init_spectrogram (char *plotTitle,
  char *xLabel,
  char *yLabel,
  int xAxisLength,
  int yAxisLength,
  double yMin,
  double yMax,
  double zMin,
  double zMax,
  char *pColourPalette,
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

  plotHandle->multiPlot2DFlag = GPC_FALSE;                      // This is NOT a 2D plot - used in gpc_close ()
  plotHandle->xAxisLength = xAxisLength;                        // Set X axis length in handle
  plotHandle->yAxisLength = yAxisLength;                        // Set Y axis length in handle
  plotHandle->numberOf2ndAxisPlotted = 0;                       // We have not plotted any columns
  strcpy (plotHandle->plotTitle, plotTitle);                    // Set plot title in handle

  fprintf (plotHandle->pipe, "set term wxt 0 title \"%s\" size %d, %d\n", plotHandle->plotTitle, CANVAS_WIDTH, CANVAS_HEIGHT); // Set the plot
  fprintf (plotHandle->pipe, "set lmargin at screen %4.8lf\n", PLOT_LMARGIN); // Define the margins so that the graph is 512 pixels wide
  fprintf (plotHandle->pipe, "set rmargin at screen %4.8lf\n", PLOT_RMARGIN);
  fprintf (plotHandle->pipe, "set border back\n");              // Set border behind plot

  fprintf (plotHandle->pipe, "set xlabel \"%s\"\n", xLabel);
  fprintf (plotHandle->pipe, "set ylabel \"%s\"\n", yLabel);

  plotHandle->yMin = yMin;                                      // Store values for min/max Y, for axis labels
  plotHandle->yMax = yMax;

                                                          // NOTE - Have to add the +/-0.5 to plot all of the pixels and
                                                          // to make peripheral pixels the same size as the internal ones
  fprintf (plotHandle->pipe, "set yrange [%lf:%lf]\n", yMin - (0.5 * ((yMax - yMin) / yAxisLength)), yMax + (0.5 * ((yMax - yMin) / yAxisLength)));  // Set length of Y axis
  fprintf (plotHandle->pipe, "set zrange [%lf:%lf] noreverse nowriteback\n", zMin, zMax);

  fprintf (plotHandle->pipe, "set tics out nomirror scale 0.75\n");  // Tics format

//  fprintf (plotHandle->pipe, "unset sur\n");                    // Remove surface lines

  if (keyMode == GPC_KEY_ENABLE)                                // Legend / key location
  {
  fprintf (plotHandle->pipe, "set key out vert nobox\n");
//    fprintf (plotHandle->pipe, "set cbrange [0:255]\n");
    fprintf (plotHandle->pipe, "unset cblabel\n");
  }
  else
  {
    fprintf (plotHandle->pipe, "unset key\n");                  // Disable legend / key
  }

  fprintf (plotHandle->pipe, "%s\n", pColourPalette);           // Colour palette

  fflush (plotHandle->pipe);                                    // flush the pipe

#if GPC_DEBUG
  sleep (1);                                                  // Slow down output so that pipe doesn't overflow when logging results
#endif

  return (plotHandle);
}


int gpc_plot_spectrogram (h_GPC_Plot *plotHandle,
  double *pData,
  char *pDataName,
  double xMin,
  double xMax)

{
  int i;

  if (pData == GPC_END_PLOT)                                    // End of plot
  {
    fprintf (plotHandle->pipe, "e\n");                          // End of spectrogram dataset
    fflush (plotHandle->pipe);                                  // Flush the pipe
    return (GPC_NO_ERROR);
  }

  if (plotHandle->numberOf2ndAxisPlotted  == 0)
  {
    fprintf (plotHandle->pipe, "set xrange [%lf:%lf]\n", xMin - (0.5 * ((xMax - xMin) / plotHandle->xAxisLength)), xMax + (0.5 * ((xMax - xMin) / plotHandle->xAxisLength)));  // Set length of X axis
    fprintf (plotHandle->pipe, "plot '-' using 1:2:3 title \"%s\" with image\n", pDataName); // Set plot format
  }

  for (i = 0; i < plotHandle->yAxisLength; i++)                 // Copy the data to gnuplot
  {
    fprintf (plotHandle->pipe, "%lf %lf %lf\n", xMin + ((((double)plotHandle->numberOf2ndAxisPlotted) * (xMax - xMin)) / ((double)(plotHandle->xAxisLength - 1))),
                                               plotHandle->yMin + ((((double)i) * (plotHandle->yMax - plotHandle->yMin)) / ((double)(plotHandle->yAxisLength - 1))),
                                               pData[i]);
  }

  if (plotHandle->numberOf2ndAxisPlotted < (plotHandle->xAxisLength - 1))
  {
    fprintf (plotHandle->pipe, "\n");                           // End of isoline scan
    plotHandle->numberOf2ndAxisPlotted++;                       // Increment number of columns plotted
 }
  else
  {
    fprintf (plotHandle->pipe, "e\n");                          // End of spectrogram dataset
    plotHandle->numberOf2ndAxisPlotted = 0;                     // Reset number of columns plotted for next scan
  }

  fflush (plotHandle->pipe);                                    // Flush the pipe

#if GPC_DEBUG
  sleep (1);                                                  // Slow down output so that pipe doesn't overflow when logging results
#endif

  return (GPC_NO_ERROR);
}

h_GPC_Plot *gpc_init_image (char *plotTitle,
  int xAxisLength,
  int yAxisLength,
  unsigned int zMin,
  unsigned int zMax,
  char *pColourPalette,
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

  plotHandle->multiPlot2DFlag = GPC_FALSE;                      // This is NOT a 2D plot - used in gpc_close ()
  plotHandle->xAxisLength = xAxisLength;                        // Set X axis length in handle
  plotHandle->yAxisLength = yAxisLength;                        // Set Y axis length in handle
  plotHandle->numberOf2ndAxisPlotted = 0;                       // We have not plotted any rows
  strcpy (plotHandle->plotTitle, plotTitle);                    // Set plot title in handle

  fprintf (plotHandle->pipe, "set term wxt 0 title \"%s\" size %d, %d\n", plotHandle->plotTitle, CANVAS_WIDTH, CANVAS_HEIGHT); // Set the plot
  fprintf (plotHandle->pipe, "set lmargin at screen %4.8lf\n", PLOT_LMARGIN); // Define the margins so that the graph is 512 pixels wide
  fprintf (plotHandle->pipe, "set rmargin at screen %4.8lf\n", PLOT_RMARGIN);
  fprintf (plotHandle->pipe, "set border back\n");              // Set border behind plot

                                                          // NOTE - Have to add the +/-0.5 to plot all of the pixels and
                                                          // to make peripheral pixels the same size as the internal ones
  fprintf (plotHandle->pipe, "set xrange [-0.5:%lf]\n", ((double)xAxisLength)-0.5);  // Set length of X axis
  fprintf (plotHandle->pipe, "set yrange [%lf:-0.5]\n", ((double)yAxisLength)-0.5);  // Set length of Y axis
  if ((zMin == GPC_IMG_AUTO_SCALE) && (zMax == GPC_IMG_AUTO_SCALE))
  {
    fprintf (plotHandle->pipe, "set autoscale  zfix\n");        // Auto-scale Z axis
  }
  else
  {
    fprintf (plotHandle->pipe, "set zrange [%d:%d]\n", zMin, zMax);
  }

  fprintf (plotHandle->pipe, "set tics out nomirror scale 0.75\n");  // Tics format

  fprintf (plotHandle->pipe, "set view map\n");                 // Set splot map view
  fprintf (plotHandle->pipe, "set size square\n");
  fprintf (plotHandle->pipe, "unset title\n");

  if (keyMode == GPC_KEY_ENABLE)                                // Legend / key location
  {
    fprintf (plotHandle->pipe, "set key out vert nobox\n");
//    fprintf (plotHandle->pipe, "set cbrange [0:255]\n");
    fprintf (plotHandle->pipe, "unset cblabel\n");
  }
  else
  {
    fprintf (plotHandle->pipe, "unset key\n");                  // Disable legend / key
  }

  fprintf (plotHandle->pipe, "%s\n", pColourPalette);           // Colour palette

  fflush (plotHandle->pipe);                                    // flush the pipe

#if GPC_DEBUG
  sleep (1);                                                  // Slow down output so that pipe doesn't overflow when logging results
#endif

  return (plotHandle);
}


int gpc_plot_image (h_GPC_Plot *plotHandle,
  unsigned char *pData,
  char *pDataName)

{
  int i, j;

  fprintf (plotHandle->pipe, "splot '-' matrix title \"%s\" with image\n", pDataName); // Set plot format

  for (j = 0; j < plotHandle->yAxisLength; j++)                 // For every row
  {
    for (i = 0; i < plotHandle->xAxisLength; i++)               // For every pixel in the row
    {
      fprintf (plotHandle->pipe, "%d ", pData[i + (j * plotHandle->xAxisLength)]);
    }
    fprintf (plotHandle->pipe, "\n");                           // End of isoline scan
  }
  fprintf (plotHandle->pipe, "\ne\ne\n");                       // End of spectrogram dataset

  fflush (plotHandle->pipe);                                    // Flush the pipe

#if GPC_DEBUG
  sleep (1);                                                 // Slow down output so that pipe doesn't overflow when logging results
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
}

