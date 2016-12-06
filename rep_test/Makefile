
OUTPUT=rep.cc.exe
OBJS=rep.s.o rep.cc.o

run: $(OUTPUT)
	gdb $(OUTPUT)

rep.cc.exe : $(OBJS)
	g++ -std=c++0x -Wall -O3 -flto -g $(OBJS) -o $(OUTPUT)

clean :
	rm -rf $(OBJS) $(OUTPUT)

rep.cc.o : rep.cc
	g++ -O3 -flto -g -c rep.cc -o rep.cc.o

rep.s.o : rep.s
	as -g -c rep.s -o rep.s.o
