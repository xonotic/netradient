# targets:
#   all
#   clean
#   radiant
#   plugins
#   game_packs
#   q3map2

.DEFAULT_GOAL := all

TYPE ?= Release

BUILD_DIR := install

CMAKE := cmake

Makefile:
	@true

clean:
	+$(CMAKE) --build $(BUILD_DIR) --target $@ -- --no-print-directory
	rm -fr $(BUILD_DIR)

$(BUILD_DIR)/.ready-$(TYPE):
	@rm -f $(BUILD_DIR)/.ready-*
	$(CMAKE) -DCMAKE_BUILD_TYPE=$(TYPE) -G "Unix Makefiles" -H. -B$(BUILD_DIR)
	@touch $(BUILD_DIR)/.ready-$(TYPE)

%: $(BUILD_DIR)/.ready-$(TYPE)
	+$(CMAKE) --build $(BUILD_DIR) --target $@


# deprecated legacy compatibility targets

install/q3map2: q3map2
	@true
install/q3map2.x86: q3map2
	@true
install/q3map2.x86_64: q3map2
	@true
