CC = g++
#CFLAGS = -c
CFLAGS = -Wall -c -O2
LDFLAGS = -lglut -lGL -lGLU -lX11 -lXmu -lXi -lm


PROG = game

CBMP = bmp.h bmp.cpp
CMD2 = md2.h md2.cpp
CPCX = pcx.h pcx.cpp
TEXU = texture.h texture.cpp
CTGX = tga.h tga.cpp


OBJS =  bmp.o md2.o pcx.o texture.o tga.o glutmain.o

$(PROG) : $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(PROG)

glutmain.o : glutmain.cpp
	$(CC) $(CFLAGS) $^ 


bmp.o : $(CBMP)
	$(CC) $(CFLAGS) $^

md2.o : $(CMD2)
	$(CC) $(CFLAGS) $^

pcx.o : $(CPCX)
	$(CC) $(CFLAGS) $^

texture.o : $(TEXU)
	$(CC) $(CFLAGS) $^

clean :
	rm *.o $(PROG)