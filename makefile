bin: steady_timer

HEADERS:=$(wildcard *.hpp)
SOURCES:=$(wildcard *.cpp)
SOURCES:= $(filter-out main.cpp, $(SOURCES))
SOURCES:= $(filter-out TimerTest.cpp, $(SOURCES))
LIB_GMOCK:= /usr/src/googletest/googlemock/make/gmock_main.a

steady_timer: $(HEADERS) $(SOURCES) main.cpp makefile
	LC_ALL=C g++ --std=c++14 $(SOURCES) main.cpp -o steady_timer
	
test: $(HEADERS) $(SOURCES) TimerTest.cpp makefile
	LC_ALL=C g++ -O0 -g3 --std=c++14 $(SOURCES) $(LIB_GMOCK) TimerTest.cpp -o test -lpthread

run: steady_timer
	./steady_timer
	
run_test: test
	GTEST_COLOR=TRUE ./test
	
