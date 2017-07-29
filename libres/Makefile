CC=agcc

OBJECTS=getaddrinfo.o
OBJECTS+=gethostbyname2_r.o
OBJECTS+=dnscruft.o
OBJECTS+=dnscruft2.o
OBJECTS+=dnscruft3.o
OBJECTS+=h_errno.o
OBJECTS+=res_query.o
OBJECTS+=res_mkquery.o
OBJECTS+=gai_strerror.o
OBJECTS+=freeaddrinfo.o
OBJECTS+=res_init.o
OBJECTS+=dn_expand.o

CFLAGS+=-Wall

all: libres.a test
clean:
	rm -f $(OBJECTS) libres.a test.o test

test: libres.a test.o
	$(CC) $(CFLAGS) -o test test.o libres.a

libres.a: $(OBJECTS)
	ar r libres.a $(OBJECTS)
	ranlib libres.a

.PHONY: all clean

%.o: %.c dietdns.h
	$(CC) $(CFLAGS) -c $<
