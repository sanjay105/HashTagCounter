hashtagcounter:hashtagcounter.o
	g++ hashtagcounter.o -o hashtagcounter
hashtagcounter.o:hashtagcounter.cpp
	g++ -c hashtagcounter.cpp
clean:
	rm *.o hashtagcounter
