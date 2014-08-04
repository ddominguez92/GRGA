#ifndef GNUPLOT_H
#define GNUPLOT_H

// Gnuplot/C interface library header file
// Please ensure that the system path includes an entry for the gnuplot binary folder

#ifndef GPC_DEBUG
  #define GPC_DEBUG                 0           // Set to '1' to enable Gnuplot text debug output, '0' otherwise
#endif

#include<stdlib.h>
#include <sys/stat.h>

#define MAX_NUM_GRAPHS          50              // Maximum number of graphs (multiplots) in a plot

#define GPC_AUTO_SCALE          0.0             // Auto scaling
#define GPC_IMG_AUTO_SCALE      0               // Auto scaling for images

#define CANVAS_WIDTH            800
#define CANVAS_HEIGHT           600
#define PLOT_LMARGIN            (80.0/CANVAS_WIDTH)   // 512 pixel X axis
#define PLOT_RMARGIN            (591.0/CANVAS_WIDTH)

#define GPC_NO_ERROR            0               // No error
#define GPC_ERROR               1               // Error

#define GPC_FALSE               0               // False flag
#define GPC_TRUE                1               // True flag

#define GPC_END_PLOT            NULL            // Flag to indicate end of the plot

enum gpcMultiFastMode                           // Multiplot / fast plot modes
{
  GPC_MULTIPLOT = 0,
  GPC_FASTPLOT
};

enum gpcKeyMode                                 // Legend / Key mode
{
  GPC_KEY_DISABLE = 0,
  GPC_KEY_ENABLE
};

enum gpcPlotSignMode                            // Sign modes - signed, positive, negative
{
  GPC_SIGNED = 0,
  GPC_POSITIVE,
  GPC_NEGATIVE
};

enum gpcNewAddGraphMode                         // New / Add graph modes
{
  GPC_NEW = 0,
  GPC_ADD
};

enum gpcPoleZeroMode                            // Pole zero plot modes
{
  GPC_COMPLEX_POLE = 0,
  GPC_CONJUGATE_POLE,
  GPC_COMPLEX_ZERO,
  GPC_CONJUGATE_ZERO
};

                                                // Spectrogram and image colour palettes
#define GPC_MONOCHROME "set palette defined (0 'black', 1 'white')"
#define GPC_COLOUR "set palette defined (0 'black', 1 'blue', 2 'red', 3 'yellow', 4 'white')"


typedef struct                                  // Complex data type
{
  double  real;
  double  imag;
} ComplexRect_s;


typedef struct
{
  char filename [40];                           // Graph filename
  char title [80];                              // Graph title
  char formatString [40];                       // Graph format string "lines", "points" etc
} h_GPC_Graph;


typedef struct
{
  FILE *pipe;                                   // Pipe to Gnuplot instance
  int numberOfGraphs;                           // Number of graphs on this plot
  h_GPC_Graph graphArray [MAX_NUM_GRAPHS];      // Array of graphs
  char plotTitle[80];                           // Plot title
  int filenameRootId;                           // Multiplot filename root Id
  enum gpcMultiFastMode multiFastMode;          // Multiplot / fast plot mode
  double scalingMode;                           // Scaling mode / dimension for XY and PZ graphs
  long xAxisLength;                             // X axis length for spectrogram plots and images
  long yAxisLength;                             // Y axis length for spectrogram plots and images
  double xMin;                                  // Minimum value of X axis - used for axis labels
  double xMax;                                  // Maximum value of X axis - used for axis labels
  double yMin;                                  // Minimum value of Y axis - used for axis labels
  double yMax;                                  // Maximum value of Y axis - used for axis labels
  long numberOf2ndAxisPlotted;                  // Number of columns for spectrogram plot
  int multiPlot2DFlag;                          // Flag set when 2D graph created
} h_GPC_Plot;


h_GPC_Plot *gpc_init_2d (char *plotTitle,       // Plot title
  char *xLabel,                                 // X axis label
  char *yLabel,                                 // Y axis label
  double scalingMode,                           // Scaling mode
  enum gpcPlotSignMode signMode,                // Sign mode - signed, positive, negative
  enum gpcMultiFastMode multiFastMode,          // Multiplot / fast plot mode
  enum gpcKeyMode keyMode);                     // Legend / key mode

int gpc_plot_2df(h_GPC_Plot *plotHandle,        // Plot handle
  float *pData,                                // Dataset pointer
  int graphLength,                              // Dataset length
  char *pDataName,                              // Dataset title
  double xMin,                                  // Minimum X value
  double xMax,                                  // Maximum X value
  char *plotType,                               // Plot type - "lines", "points", "impulses", "linespoints"
  char *pColour,                                // Colour - Use gnuplot> show colornames to see available colours
  enum gpcNewAddGraphMode addMode);             // Add / new mode


int gpc_plot_2dd(h_GPC_Plot *plotHandle,        // Plot handle
  double *pData,                                // Dataset pointer
  int graphLength,                              // Dataset length
  char *pDataName,                              // Dataset title
  double xMin,                                  // Minimum X value
  double xMax,                                  // Maximum X value
  char *plotType,                               // Plot type - "lines", "points", "impulses", "linespoints"
  char *pColour,                                // Colour - Use gnuplot> show colornames to see available colours
  enum gpcNewAddGraphMode addMode);             // Add / new mode

h_GPC_Plot *gpc_init_xy (char *plotTitle,       // Plot title
  char *xLabel,                                 // X axis label
  char *yLabel,                                 // Y axis label
  double dimension,                             // Dimension - this is square
  enum gpcKeyMode keyMode);                     // Legend / key mode

int gpc_plot_xy (h_GPC_Plot *plotHandle,        // Plot handle
  ComplexRect_s *pData,                         // Dataset pointer
  int graphLength,                              // Dataset length
  char *pDataName,                              // Dataset title
  char *plotType,                               // Plot type - "lines", "points", "impulses", "linespoints"
  char *pColour,                                // Colour - Use gnuplot> show colornames to see available colours
  enum gpcNewAddGraphMode addMode);             // Add / new mode

h_GPC_Plot *gpc_init_pz (char *plotTitle,       // Plot title
  double dimension,                             // Dimension - this is square
  enum gpcKeyMode keyMode);                     // Legend / key mode

int gpc_plot_pz (h_GPC_Plot *plotHandle,        // Plot handle
  ComplexRect_s *pData,                         // Dataset pointer
  int graphLength,                              // Dataset length
  char *pDataName,                              // Dataset title
  enum gpcPoleZeroMode poleZeroMode,            // Pole-zero mode
  enum gpcNewAddGraphMode addMode);             // Add / new mode

h_GPC_Plot *gpc_init_spectrogram (char *plotTitle,  // Plot title
  char *xLabel,                                 // X axis label
  char *yLabel,                                 // Y axis label
  int xAxisLength,                              // X axis length
  int yAxisLength,                              // Y axis length
  double yMin,                                  // Minimum Y value
  double yMax,                                  // Maximum Y value
  double zMin,                                  // Minimum Z value
  double zMax,                                  // Maximum Z value
  char *colourPalette,                          // Colour palette
  enum gpcKeyMode keyMode);                     // Legend / key mode

int gpc_plot_spectrogram (h_GPC_Plot *plotHandle,   // Plot handle
  double *pData,                                // Dataset pointer
  char *pDataName,                              // Dataset title
  double xMin,                                  // Minimum X value
  double xMax);                                 // Maximum X value

h_GPC_Plot *gpc_init_image (char *plotTitle,    // Plot title
  int xAxisLength,                              // X axis length
  int yAxisLength,                              // X axis length
  unsigned int zMin,                            // Minimum Z value
  unsigned int zMax,                            // Maximum Z value
  char *colourPalette,                          // Colour palette
  enum gpcKeyMode keyMode);                     // Legend / key mode

int gpc_plot_image (h_GPC_Plot *plotHandle,     // Plot handle
  unsigned char *pData,                         // Dataset pointer
  char *pDataName);                             // Dataset title

void gpc_close (h_GPC_Plot *);                  // Plot handle

#endif
