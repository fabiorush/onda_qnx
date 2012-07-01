ROOT_DIR := $(notdir $(CURDIR))
ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

HOST_MKIFS := mkifs

SUFFIXES := .build .ifs .raw

.PHONY: all clean

all: ifs-omap3730-beagle.bin 

clean: 
	$(RM_HOST) ifs-omap3730-beagle.bin 

ifs-omap3730-beagle.bin: omap3730_beagle_xm.omap3730_beagle_xm.build
	$(HOST_MKIFS) -r../install -v $(MKIFSFLAGS) $^ $@

