
CXX=g++
CXXFLAGS=-Wall -Wextra -Wpedantic -Werror -Wfatal-errors -Winit-self -Wshadow -Wlogical-op -Wcomment -Wctor-dtor-privacy -Wold-style-cast -D_GLIBCXX_DEBUG -fno-diagnostics-show-option

all: Scheduler.cc 
	$(CXX) $(CXXFLAGS) -o Scheduler Scheduler.cc
		
clean:
	rm -f Scheduler *.o *.zip
	
package:
	zip Nicholson-Edge-HW5.zip README.txt Makefile Scheduler.cc
