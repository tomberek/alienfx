LBITS := $(shell getconf LONG_BIT)
ifeq ($(LBITS),64)
   ALIENFX_BUILD := 64bit
else
   ALIENFX_BUILD := 32bit
endif


all:  build$(ALIENFX_BUILD)

build64bit: alienfx.cpp
	g++ -g -fPIC -lusb-1.0 -lpthread -o alienfx64 alienfx.cpp
	cp alienfx64 alienfx

build32bit: alienfx.cpp
	g++ -m32 -I/usr/include/libusb-1.0 -g -fPIC -lusb-1.0 -lpthread -o alienfx32 alienfx.cpp
	cp alienfx32 alienfx

install:  install$(ALIENFX_BUILD)

install64bit:
	install -o root -g root -m 4755 alienfx64 /usr/bin/alienfx

install32bit:
	install -o root -g root -m 4755 alienfx32 /usr/bin/alienfx

deb:
	equivs-build --full dpkg

tarball:
	rm source.tar.gz
	cd .. && tar cvzf source.tar.gz alienfx/ && mv source.tar.gz alienfx/ && cd alienfx/

man:
	groff -Tascii -man alienfx.1 | less
