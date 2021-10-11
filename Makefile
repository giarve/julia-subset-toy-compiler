CODE_DIR = src

.PHONY: source_subdir

source_subdir:
	$(MAKE) -C $(CODE_DIR)

clean:
	$(MAKE) -C $(CODE_DIR) clean