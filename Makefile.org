CC = cc

CFLAGS = -O

TARGET = nuweb

OBJS = main.o pass1.o latex.o output.o input.o scraps.o names.o \
	arena.o global.o

.SUFFIXES: .tex .dvi .w

.w.tex:
	nuweb $*

.tex.dvi:
	latex $*

.w.dvi:
	$(MAKE) $*.tex
	$(MAKE) $*.dvi

all:
	$(MAKE) $(TARGET).tex
	$(MAKE) $(TARGET)

shar:	$(TARGET).tex
	cp blurb nuweb.tex
	shar -o $(TARGET).shar Makefile README literate.bib nuweb.w \
		nuweb.tex arena.c input.c latex.c main.c names.c \
		output.c pass1.c scraps.c global.c global.h nuweb.bbl
	rm -f nuweb.tex

clean:
	rm -f *.tex *.log *.dvi *~ *.blg

veryclean:
	rm -f *.o *.c *.h *.tex *.log *.dvi *~ *.blg

view:	$(TARGET).dvi
	xdvi $(TARGET).dvi

print:	$(TARGET).dvi
	lpr -d $(TARGET).dvi

$(OBJS): global.h

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)
