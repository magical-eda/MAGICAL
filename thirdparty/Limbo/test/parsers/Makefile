all: bookshelf def ebeam gdf gdsii lef lp tf verilog
.PHONY: bookshelf def ebeam gdf gdsii lef lp tf verilog

bookshelf:
	$(MAKE) -C bookshelf

def:
	$(MAKE) -C def

ebeam:
	$(MAKE) -C ebeam

gdf:
	$(MAKE) -C gdf

gdsii:
	$(MAKE) -C gdsii

lef:
	$(MAKE) -C lef

lp:
	$(MAKE) -C lp

tf:
	$(MAKE) -C tf

verilog:
	$(MAKE) -C verilog

.PHONY: clean
clean:
	$(MAKE) clean -C bookshelf
	$(MAKE) clean -C def
	$(MAKE) clean -C ebeam
	$(MAKE) clean -C gdf
	$(MAKE) clean -C gdsii
	$(MAKE) clean -C lef
	$(MAKE) clean -C lp
	$(MAKE) clean -C tf
	$(MAKE) clean -C verilog

.PHONY: extraclean
extraclean:
	$(MAKE) extraclean -C bookshelf
	$(MAKE) extraclean -C def
	$(MAKE) extraclean -C ebeam
	$(MAKE) extraclean -C gdf
	$(MAKE) extraclean -C gdsii
	$(MAKE) extraclean -C lef
	$(MAKE) extraclean -C lp
	$(MAKE) extraclean -C tf
	$(MAKE) extraclean -C verilog
