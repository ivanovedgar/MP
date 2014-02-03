LDLIBS = -lboost_program_options

all : ptutils

ptutils : PTutils.cpp uart.c uart.h
	g++ -o $@ PTutils.cpp uart.c $(LDLIBS)

clean :
	rm *.o ptutils
