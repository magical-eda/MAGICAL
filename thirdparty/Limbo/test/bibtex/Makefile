#==========================================================================
#                         Directories and names 
# ==========================================================================

LIMBO_ROOT_DIR = $(realpath ../..)
TEST_DIR = $(realpath .)

# ==========================================================================
#                         Standard Setting
# ==========================================================================

all: web cv

# test web 
web: $(LIMBO_ROOT_DIR)/limbo/bibtex/bibconvert.py $(TEST_DIR)/benchmarks/Top.bib $(TEST_DIR)/benchmarks/publications.bib 
	@echo ">> test web"
	@python $(LIMBO_ROOT_DIR)/limbo/bibtex/bibconvert.py --suffix web --highlight "Yibo Lin" --highlight "Lin, Yibo" --input $(TEST_DIR)/benchmarks/Top.bib --input $(TEST_DIR)/benchmarks/publications.bib 

# test cv 
cv: $(LIMBO_ROOT_DIR)/limbo/bibtex/bibconvert.py $(TEST_DIR)/benchmarks/Top.bib $(TEST_DIR)/benchmarks/publications.bib 
	@echo ">> test cv"
	@python $(LIMBO_ROOT_DIR)/limbo/bibtex/bibconvert.py --suffix cv --highlight "Yibo Lin" --highlight "Lin, Yibo" --input $(TEST_DIR)/benchmarks/Top.bib --input $(TEST_DIR)/benchmarks/publications.bib 
