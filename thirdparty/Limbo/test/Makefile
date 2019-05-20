all: algorithms geometry parsers programoptions solvers string

.PHONY: algorithms geometry parsers programoptions solvers string

algorithms:
	$(MAKE) -C algorithms

geometry:
	$(MAKE) -C geometry

parsers:
	$(MAKE) -C parsers

programoptions:
	$(MAKE) -C programoptions

solvers:
	$(MAKE) -C solvers

string:
	$(MAKE) -C string

.PHONY: clean
clean:
	$(MAKE) clean -C algorithms
	$(MAKE) clean -C geometry
	$(MAKE) clean -C parsers
	$(MAKE) clean -C programoptions
	$(MAKE) clean -C solvers
	$(MAKE) clean -C string

.PHONY: extraclean
extraclean:
	$(MAKE) extraclean -C algorithms
	$(MAKE) extraclean -C geometry
	$(MAKE) extraclean -C parsers
	$(MAKE) extraclean -C programoptions
	$(MAKE) extraclean -C solvers
	$(MAKE) extraclean -C string
