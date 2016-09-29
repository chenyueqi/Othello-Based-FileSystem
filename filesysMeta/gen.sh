g++ -o filter filter.cc
g++ -o enlarge enlarge.cc
g++ -o mkdirFilter mkdirFilter.cc
g++ -o touchFilter touchFilter.cc
g++ -o writeFilter writeFilter.cc
g++ -o readFilter readFilter.cc
g++ -o lsFilter lsFilter.cc

./filter metadata briefdata
./enlarge briefdata enlargeddata
./mkdirFilter enlargeddata mkdirTestcase
./touchFilter enlargeddata touchTestcase
./writeFilter enlargeddata writeTestcase
./readFilter enlargeddata readTestcase
./lsFilter enlargeddata lsTestcase

rm briefdata
rm enlargeddata

rm filter
rm enlarge
rm mkdirFilter
rm touchFilter
rm writeFilter
rm readFilter
rm lsFilter

mv mkdirTestcase ../testcase
mv touchTestcase ../testcase
mv writeTestcase ../testcase
mv readTestcase ../testcase
mv lsTestcase ../testcase
