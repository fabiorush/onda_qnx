define PINFO
PINFO DESCRIPTION=Graphics driver dll for OMAP 35xx LCD controller - loaded by io-display
endef
EXTRA_CCDEPS += $(PROJECT_ROOT)/$(SECTION)/omap.h
SHARED_LIBS += cacheS
