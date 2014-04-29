all:
	rm -f *.a
	LC_ALL=en_US g++ -c PanNTilt.cpp PTfunctions.cpp uart.cpp
	ar -cvq libPTUTASS.a *.o
	LC_ALL=en_US g++ -o PTUMain main.cpp libPTUTASS.a
	rm -f *.o core