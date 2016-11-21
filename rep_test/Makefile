
OBJS=rep.s.o rep.cc.o

run: rep.exe
	gdb ./rep.exe

rep.exe : $(OBJS)
	g++ -O3 -flto -g $(OBJS) -o rep.exe

clean :
	rm -rf $(OBJS)

rep.cc.o : rep.cc
	g++ -O3 -flto -g -c rep.cc -o rep.cc.o

rep.s.o : rep.s
	as -g -c rep.s -o rep.s.o
