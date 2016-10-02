#compile execution file
g++ -o filter filter.cc
g++ -o enlarge enlarge.cc

g++ -o touchFilter touchFilter.cc
g++ -o writeFilter writeFilter.cc
g++ -o readFilter readFilter.cc
g++ -o rmFilter rmFilter.cc
g++ -o mvFilter mvFilter.cc
g++ -o cpFilter cpFilter.cc

g++ -o mkdirFilter mkdirFilter.cc
g++ -o lsFilter lsFilter.cc
g++ -o mvrFilter mvrFilter.cc
g++ -o rmrFilter rmrFilter.cc

#generate enlagred metadata and testcase
./filter metadata briefdata
./enlarge briefdata enlargeddata

./touchFilter enlargeddata touchTestcase
./writeFilter enlargeddata writeTestcase
./readFilter enlargeddata readTestcase
./rmFilter enlargeddata rmTestcase
./mvFilter enlargeddata mvTestcase
./cpFilter enlargeddata cpTestcase

./mkdirFilter enlargeddata mkdirTestcase
./lsFilter enlargeddata lsTestcase
./mvrFilter enlargeddata mvrTestcase
./rmrFilter enlargeddata rmrTestcase

#move testcase to relevant directory
mv touchTestcase ../testcase
mv writeTestcase ../testcase
mv readTestcase ../testcase
mv rmTestcase ../testcase
mv mvTestcase ../testcase
mv cpTestcase ../testcase

mv mkdirTestcase ../testcase
mv lsTestcase ../testcase
mv mvrTestcase ../testcase
mv rmrTestcase ../testcase

#delete execution file and generated metadata
rm filter
rm enlarge
rm touchFilter
rm writeFilter
rm readFilter
rm rmFilter
rm mvFilter
rm cpFilter

rm mkdirFilter
rm lsFilter
rm mvrFilter
rm rmrFilter

rm briefdata
rm enlargeddata
