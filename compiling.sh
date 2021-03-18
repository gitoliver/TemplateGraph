#g++ -std=c++17 -I $GEMSHOME/gmml/includes/ -I includes/ -L$GEMSHOME/gmml/bin/ -Wl,-rpath,$GEMSHOME/gmml/bin/ src/*.cpp -lgmml -o gp_builder
g++ -std=c++17 -I includes/ -I includes/Abstract/ -Wl,-rpath, src/Abstract/*.cpp testDriver.cpp -o runMe
printf "Now do:\n./runMe\n"
