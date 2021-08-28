LDFLAGS := -lcurses

examples: simple1 simple2
.PHONY: examples

simple1: simple1.cpp
	$(CXX) $(LDFLAGS) $^ -o $@

simple2: simple2.cpp
	$(CXX) $(LDFLAGS) $^ -o $@

docs: html latex
.PHONY: docs

html:
	doxygen

latex:
	doxygen

clean:
	rm -rf html latex simple[12]
