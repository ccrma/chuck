###############################################################################
# name: makefile
# desc: unit testing for ChucK programming language
#
# ChucK website: https://chuck.stanford.edu/
# source repository: https://github.com/ccrma/chuck/
###############################################################################
.PHONY: all
all: test


############################## RUN UNIT TEST ##################################
.PHONY: test
test:
ifneq (,$(wildcard ../chuck ../chuck.exe))
	./test.py ../chuck .
else
	@echo "no chuck executable found in 'src/'...(hint: make in 'src/')"
endif


############################## CLEAN UP #######################################
.PHONY: clean
clean:
	@echo "removing generated test files..."
	@rm -f 03-Modules/01-file-io.bin 03-Modules/08-write-read-op.txt \
	03-Modules/09-write-output.txt 03-Modules/file.bin \
