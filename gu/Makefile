main: main.o gu.o shrdlu.o sheet.o
	g++  main.o gu.o shrdlu.o sheet.o -o gu -lboost_system -lboost_filesystem -lboost_program_options
	./gu *.csv

main.o: main.cc
	g++ -c -std=c++11 main.cc

install:
	mv gu /usr/local/bin

test: test_shrdlu test_sheet test_gu
	./test_gu

test_gu: test_gu.o gu.o shrdlu.o sheet.o
	g++ -otest_gu -lboost_unit_test_framework test_gu.o shrdlu.o sheet.o gu.o

test_gu.o: test_gu.cc
	g++ -c -std=c++11 test_gu.cc

test_shrdlu: test_shrdlu.o shrdlu.o
	g++ -otest_shrdlu -lboost_unit_test_framework test_shrdlu.o shrdlu.o

test_shrdlu.o: test_shrdlu.cc
	g++ -c -std=c++11 test_shrdlu.cc

test_sheet: test_sheet.o sheet.o
	g++ -otest_sheet -lboost_unit_test_framework test_sheet.o sheet.o

test_sheet.o: test_sheet.cc
	g++ -c -std=c++11 test_sheet.cc

sheet.o: sheet.cc
	g++ -c -std=c++11 sheet.cc

gu.o: gu.cc
	g++ -c -std=c++11 gu.cc

shrdlu.o: shrdlu.cc
	g++ -c -std=c++11 shrdlu.cc

clean:
	rm *.o main test_gu test_shrdlu test_sheet
