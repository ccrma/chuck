current: 
	@echo "[ckdl build]: please use one of the following configurations:"; echo "   make linux, make osx, or make win32"

install:
	@echo "(must be root to install)"
	cp $(wildcard ckdl ckdl.exe) /usr/bin/

osx:
	-make -f makefile.osx

linux:
	-make -f makefile.linux

win32: 
	-make -f makefile.win32

common: 
	-make -f makefile.common

clean:
	rm -f *.o *.ckx $(wildcard ckdl ckdl.exe)
