CCFLAGS=--std=gnu++98 -pedantic -Wall -Werror -ggdb3
myShell: myShell.o myShellclass.o
	g++ -o myShell $(CCFLAGS) myShell.o myShellclass.o

%.o: %.cpp myShellclass.h
	g++ -c $(CCFLAGS) $<

clean:
	rm -f *.o  *~ myShell

