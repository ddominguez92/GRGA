CXX_FLAGS="-fopenmp -D_REENTRANT -std=c++11 -lgomp -fopenmp"
    
g++ -c parameters.cpp $CXX_FLAGS
g++ -c stack.cpp $CXX_FLAGS
g++ -c gp.cpp $CXX_FLAGS
g++ -c evaluation.cpp $CXX_FLAGS
g++ -c generation.cpp $CXX_FLAGS
g++ -c plot.cpp $CXX_FLAGS
g++ -c direction.cpp $CXX_FLAGS
g++ -c tournament.cpp $CXX_FLAGS
g++ -c gnuplot_c.cpp $CXX_FLAGS
g++ -c es.cpp $CXX_FLAGS
g++ -c main.cpp $CXX_FLAGS

g++ parameters.o stack.o gp.o evaluation.o generation.o plot.o direction.o tournament.o es.o gnuplot_c.o main.o -fopenmp -D_REENTRANT -std=c++11 -lgomp -fopenmp -o grga

mv *.o ./bin/
mv grga ./bin/

