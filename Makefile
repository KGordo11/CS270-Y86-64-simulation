HDRFILES = y86sim.h
CFLAGS = -g
SIMFILES = main.o cond.o fetch.o decode.o execute.o memory.o \
	writeback.o pcupdate.o
TARFILES =decode.c execute.c memory.c writeback.c pcupdate.c cond.c

ysim:	$(HDRFILES) $(SIMFILES)
	$(CC) -o ysim $(SIMFILES)

clean:
	$(RM) *.o ysim

submit:
	tar cvf p3submission.tar $(TARFILES)
