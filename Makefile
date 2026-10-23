SRC      := filter.c
PROVERS  ?= alt-ergo
TIMEOUT  ?= 30
SESSION  := lemma-rocq
WP       := frama-c -wp -wp-prover $(PROVERS) -wp-timeout $(TIMEOUT)

.PHONY: all prove lemmas filter gui run clean

all: prove

# Prove lemmas, then the filter function
prove: lemmas filter

# Check the lemmas' Coq proofs saved in $(SESSION)/interactive/ (batch mode)
lemmas:
	frama-c -wp -wp-prop @lemma -wp-prover coq -wp-session $(SESSION) $(SRC)

# Prove only the filter function (with RTE guards)
filter:
	$(WP) -wp-rte -wp-fct filter $(SRC)

# Open the Frama-C GUI for interactive proofs
gui:
	ivette -wp -wp-rte -wp-skip-fct main $(SRC)

# Compile and run as a plain C program
run: filter.out
	./filter.out

filter.out: $(SRC)
	gcc -Wall -o $@ $<

clean:
	rm -rf .frama-c filter.out
