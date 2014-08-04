# Symbolic Regression Engine

This project was developed by Daniel Dominguez Catena in the year 2014, as part of his final project on the Degree on Computer Science of the Public University of Navarre (UPNA).

The code is given as is, with no warranty whatsoever. Any kind of contribution will be wellcome.

## Introduction

This software is a Symbolic Regression Engine for function discovery. It was designed with a memethic algorythm approach, mixing a genetic programming algorithm with a local improvement evolution strategy.

It will answer with functions to data, attempting to get a minimum error result for a given dataset.

## Prerequisites

Gnuplot
GCC g++ with c++11 extension

## Compilation

Just run the `compile.sh` script. The output will be put on the `/bin` folder.

## Configuration

A sample config is given in the `/bin/input` folder. The tasks file contains the data to analyze, and the other files the configuration of the engine.

## Running

Just run the `grga` executable on the `/bin` folder, as is. It will look for its tasks.txt file and run with the specified configuration.


## Please be noted

The corpus of the dissertation where most of this code was explained is in spanish, and won't be directly uploaded to the internet within a few months. The code is uncommented and quite complex, so it's not advised going straight into it (send me an email first if you are interested).

With more time I will try to comment at least the configuration files. If you need to use this software and have any problem with it just send me a message and I will be happy to help you configure it.

If you need something more powerful, you can look for the commercial software Eureqa by Nutonian at
http://www.nutonian.com/products/eureqa/

## License

GRGA is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

GRGA is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GRGA. If not, see <http://www.gnu.org/licenses/>.
