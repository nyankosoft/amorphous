#CFLAGS = -O2 -fno-exceptions -Xlinker --allow-multiple-definition

fixed_string_test: Timer.o fixed_string_test.o
	g++ Timer.o fixed_string_test.o -o fixed_string_test

Timer.o: ../../../Project1/amorphous/Support/Timer_posix.cpp
	g++ -c -I../../../Project1 ../../../Project1/amorphous/Support/Timer_posix.cpp -o Timer.o

fixed_string_test.o: fixed_string_test.cpp
	g++ -c -I../../../Project1 fixed_string_test.cpp -o fixed_string_test.o
