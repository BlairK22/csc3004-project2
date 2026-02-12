CC= g++
CFLAGS= -g -std=c++11
USER = blakaramaga
WEBDIR = /var/www/html/class/csc3004/$(USER)
CGIDIR = $(WEBDIR)/cgi-bin

all: deploy

Ref.o: Ref.cpp Ref.h
	$(CC) $(CFLAGS) -c Ref.cpp

Verse.o: Verse.cpp Verse.h Ref.h
	$(CC) $(CFLAGS) -c Verse.cpp

Bible.o: Bible.cpp Bible.h Ref.h Verse.h
	$(CC) $(CFLAGS) -c Bible.cpp

bibleajax.o: bibleajax.cpp Bible.h Ref.h Verse.h
	$(CC) $(CFLAGS) -c bibleajax.cpp

bibleajax.cgi: bibleajax.o Bible.o Ref.o Verse.o
	$(CC) $(CFLAGS) -o bibleajax.cgi bibleajax.o Bible.o Ref.o Verse.o

deploy: bibleajax.cgi
	cp bibleajax.cgi $(CGIDIR)/bibleajax.cgi
	chmod 755 $(CGIDIR)/bibleajax.cgi
	cp bibleajax.html $(WEBDIR)/bibleajax.html

clean:
	rm -f *.o bibleajax.cgi core
