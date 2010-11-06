DESTDIR=/usr/bin

current: 
	@echo "[chuck build]: please use one of the following configurations:"; echo "   make linux-alsa, make linux-jack, make linux-oss,"; echo "   make osx, make osx-ub, or make win32"

install:
	cp $(wildcard chuck chuck.exe) $(DESTDIR)/
	chmod 755 $(DESTDIR)/$(wildcard chuck chuck.exe)

osx:
	-make -f makefile.osx

osx-ppc:
	-make -f makefile.osx

osx-intel:
	-make -f makefile.osx

# Mac OS X universal binary
osx-ub:
	-make -f makefile.osx-ub

linux-oss: 
	-make -f makefile.oss 

linux-jack:
	-make -f makefile.jack

linux-alsa: 
	-make -f makefile.alsa

win32: 
	-make -f makefile.win32

osx-rl:
	-make -f makefile.rl

clean:
	rm -f *.o chuck.tab.c chuck.tab.h chuck.yy.c chuck.output $(wildcard chuck chuck.exe)
