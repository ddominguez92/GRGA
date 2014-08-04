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




With more time I will try to comment at least the configuration files. If you need to use this software and have any problem with it just send me a message and I will be happy to help you configure it.
