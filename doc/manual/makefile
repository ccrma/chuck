# ChucK manual makefile
# Adam R. Tindale --art 2005
#
# make pdf will latex twice and open (OSX only)
# make clean gets rid of a lot of nasty bits

pdf: latex latex2

latex:
	pdflatex ChucK_manual.tex

latex2:
	pdflatex ChucK_manual.tex

open:
	open ChucK_manual.pdf



clean:
	rm -f .DS_Store chapters/.DS_Store *aux *log *toc *out chapters/*aux *~ chapters/*~ styles/*~ 
