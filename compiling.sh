#export GEMSHOME=YOUR/PATH/gems
#g++ -std=c++17 -I $GEMSHOME/gmml/includes/ -I includes/ -L$GEMSHOME/gmml/bin/ -Wl,-rpath,$GEMSHOME/gmml/bin/ src/*.cpp -lgmml -o gp_builder
g++ -std=c++17 -I includes/ -I includes/Abstract/ -Wl,-rpath, src/*.cpp src/Abstract/*.cpp -o graphMain
