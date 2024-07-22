###############################################################################
# name: makefile
# desc: API Reference generation (using CKDoc) for ChucK
#
# ChucK website: https://chuck.stanford.edu/
# source repository: https://github.com/ccrma/chuck/
#
# currently live API Reference:
#     API Reference: https://chuck.stanford.edu/doc/reference/
###############################################################################
all: doc


########################## GENERATE API REFERENCE #############################
.PHONY: doc ckdoc
doc ckdoc:
ifneq (,$(wildcard ../../chuck ../../chuck.exe))
	@echo "generating ChucK API reference using ckdoc..."
	../../chuck gen-all.ck
else
	@echo "no chuck executable found in 'src/'...(hint: make in 'src/')"
endif


############################## CLEAN UP #######################################
.PHONY: clean
clean:
	@echo "removing generated API reference files..."
	@rm -f *.css *.html *.json
