markov: markov_text.cpp
	g++ -O3 -std=c++11 markov_text.cpp -o markov -lboost_system -lboost_filesystem -lboost_program_options

anagrams: anagrams.cpp
	g++ -O3 -std=c++11 anagrams.cpp -o anagrams

all: markov anagrams

clean: 
	rm *.o markov anagrams a.out
