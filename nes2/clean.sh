for i in $(ls *.cpp) 
do
  echo "Cleaning $i"
  rm build/$i.o
done