###############################################################################
# name: makefile
# desc: CK Doc / API Reference generation for ChucK programming language
#
# ChucK website: https://chuck.stanford.edu/
# source repository: https://github.com/ccrma/chuck/
#
# currently live API Reference:
# API Reference: https://chuck.stanford.edu/doc/reference/
###############################################################################

all: ckdoc

############################## RUN UNIT TEST ##################################
ckdoc:
ifneq (,$(wildcard ../../chuck ../../chuck.exe))
	@echo "generating ChucK API reference using ckdoc..."
	../../chuck gen-all.ck
else
	@echo "no chuck executable found in 'src/'...(hint: make in 'src/')"
endif


############################## CLEAN UP #######################################
clean:
	@echo "removing generated API reference files..."
	@rm -f *.css *.html *.json
