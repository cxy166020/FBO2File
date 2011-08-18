CC = g++
#CFLAGS = -c
CFLAGS = -Wall -c -O2
LDFLAGS = -lglut -lGL -lGLU #-lX11 -lXmu -lXi -lm


PROG = game

CBMP = bmp.h bmp.cpp
CMD2 = md2.h md2.cpp
CPCX = pcx.h pcx.cpp
TEXU = texture.h texture.cpp
CTGX = tga.h tga.cpp
FBOU = fboUtils.h fboUtils.cpp
GINF = glInfo.h glInfo.cpp
CIMG = CImage.h CImage.cpp
CMIX = imageMix.h imageMix.cpp


OBJS =  bmp.o md2.o pcx.o texture.o tga.o glutmain.o fboUtils.o glInfo.o CImage.o imageMix.o

$(PROG) : $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(PROG)

glutmain.o : glutmain.cpp
	$(CC) $(CFLAGS) $^ 

CImage.o : $(CIMG)
	$(CC) $(CFLAGS) $^

imageMix.o : $(CMIX)
	$(CC) $(CFLAGS) $^

bmp.o : $(CBMP)
	$(CC) $(CFLAGS) $^

md2.o : $(CMD2)
	$(CC) $(CFLAGS) $^

pcx.o : $(CPCX)
	$(CC) $(CFLAGS) $^

texture.o : $(TEXU)
	$(CC) $(CFLAGS) $^

fboUtils.o : $(FBOU)
	$(CC) $(CFLAGS) $^

glInfo.o : $(GINF)
	$(CC) $(CFLAGS) $^

clean :
	rm *.o $(PROG)