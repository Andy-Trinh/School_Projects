TESTFILES := $(wildcard *.a)
TESTS := $(TESTFILES:.a=.test)

.PHONY: all

all: $(TESTS)

%.test:
	@rm -f $*.unparse $*.err
	@touch $*.unparse
	@echo "TEST $*"
	@../ac $*.a -u $*.unparse 2> $*.err ;\
	PROG_EXIT_CODE=$$?;\
	if [ $$PROG_EXIT_CODE != 0 ]; then \
		echo "ac error:"; \
		cat $*.err; \
		exit 1; \
	fi; \
	diff -B --ignore-all-space $*.unparse $*.unparse.expected; \
	STDOUT_DIFF_EXIT=$$?;\
	exit $$STDOUT_DIFF_EXIT || echo "Tests passed"

clean:
	rm -f *.unparse *.err
