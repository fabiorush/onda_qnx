ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

define PINFO
PINFO DESCRIPTION=SDIO library
endef

ifndef USEFILE
USEFILE=
endif
EXTRA_SRCVPATH += $(PROJECT_ROOT)/hc

-include $(PROJECT_ROOT)/roots.mk
#####AUTO-GENERATED by packaging script... do not checkin#####
   INSTALL_ROOT_nto = $(PROJECT_ROOT)/../../../../install
   USE_INSTALL_ROOT=1
##############################################################

NAME=sdio
include $(MKFILES_ROOT)/qmacros.mk
-include $(PROJECT_ROOT)/$(SECTION)/pinfo.mk
include $(MKFILES_ROOT)/qtargets.mk
