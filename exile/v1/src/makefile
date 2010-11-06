current: 
	@echo "[chuck build]: please use one of the following configurations:"; echo "   make linux-alsa, make linux-jack, make linux-oss, make osx, or make win32"

install:
	@echo "(must be root to install)"
	cp $(wildcard chuck chuck.exe) /usr/bin/; chmod 755 /usr/bin/$(wildcard chuck chuck.exe)

osx: 
	-make -f makefile.osx

linux-oss: 
	-make -f makefile.oss 

linux-jack:
	-make -f makefile.jack

linux-alsa: 
	-make -f makefile.alsa

win32: 
	-make -f makefile.win32

clean:
	rm -f *.o chuck.tab.c chuck.tab.h chuck.yy.c chuck.output $(wildcard chuck chuck.exe)
