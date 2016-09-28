g++ -o filter filter.cc
g++ -o enlarge enlarge.cc
g++ -o mkdirFilter mkdirFilter.cc

./filter metadata briefdata
./enlarge briefdata enlargeddata
./mkdirFilter enlargeddata mkdirTestcase

rm briefdata
rm enlargeddata

rm filter
rm enlarge
rm mkdirFilter

mv mkdirTestcase ../testcase
