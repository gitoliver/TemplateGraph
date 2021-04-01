#g++ -std=c++17 -I $GEMSHOME/gmml/includes/ -I includes/ -L$GEMSHOME/gmml/bin/ -Wl,-rpath,$GEMSHOME/gmml/bin/ src/*.cpp -lgmml -o gp_builder
clear; clear;
#Remove old executable
if [ -f runMe ]; then
    rm runMe
fi
#Create log directory if necessary
if [ ! -d logs ]; then
    mkdir logs/
fi
#Compile
g++ -std=c++17 -I includes/ -I GenericObject/includes/ -Wl,-rpath, GenericObject/src/*.cpp testDriver.cpp -o runMe
#Check if compiled ok
if [ $? -ne 0 ]; then
	printf "COMPILE FAILED!!!\n".
else
	printf "Compile complete. Running runMe\n"
    dateToday=`date +%F_%T`
	./runMe > logs/$dateToday.log
    cat logs/$dateToday.log
fi