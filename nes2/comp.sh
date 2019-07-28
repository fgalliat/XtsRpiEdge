clear
 mkdir build 2>/dev/null

#MODE="DESKTOP"
MODE="XTSCONSOLE"
if [ "$1" = "arm" ]     
then                    
 # Arietta version      
 . ../cpp/arch/rpi/rpi.arch      
else                    
 # Desktop version      
 . ../cpp/arch/desktop/desktop.arch      
fi                      

DEPS="$CPPs"

echo "Mode : $MODE"

for i in $DEPS
do
 echo "Compiling CORE $i"
 if [ -f build/$(basename $i).o ] 
 then
   echo "Skip."
 else
   g++ -std=c++11 -fPIC -fpermissive $DIRECTIVES -I include -c ../$i -o build/$(basename $i).o
 fi
done


#FLAGS="-O3 -march=native -std=c++14 -Wno-unused-value"
#CPP="clang++"

FLAGS="-std=c++11 -Wno-unused-value"
CPP="g++"

for i in $( ls *.cpp ) 
do
 echo "Compiling $i"
 $CPP $FLAGS -fPIC -I include -I lib/include -I simpleini -D$MODE -c $i -o build/$i.o
done

for i in $( ls ./lib/*.cpp ) 
do
if [ -f build/$(basename $i).o ] 
 then
   echo "Skip."
 else
   echo "Compiling LIB $i"
   $CPP $FLAGS -fPIC -I include -I lib/include -I simpleini -D$MODE -c $i -o build/$(basename $i).o
fi 
done

for i in $( ls ./mappers/*.cpp ) 
do
if [ -f build/$(basename $i).o ] 
 then
   echo "Skip."
 else
   echo "Compiling MAPPER $i"
   $CPP $FLAGS -fPIC -I include -I lib/include -I simpleini -D$MODE -c $i -o build/$(basename $i).o
 fi
done

echo "Linking whole"
 # g++ $FLAGS build/*.o -o ./laines -lSDL2 -lSDL2_image -lSDL2_ttf -lpthread
 g++ $FLAGS build/*.o -o ./laines $LIBS
