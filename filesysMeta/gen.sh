g++ -o filter filter.cc
g++ -o enlarge enlarge.cc
g++ -o mkdirFilter mkdirFilter.cc

./filter
./enlarge
./mkdirFilter

rm briefdata
rm enlargeddata

rm filter
rm enlarge
rm mkdirFilter

mv mkdirTestcase ../testcase
