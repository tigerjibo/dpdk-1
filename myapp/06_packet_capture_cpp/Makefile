

ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, can be overridden by command line or environment
RTE_TARGET ?= x86_64-native-linuxapp-gcc

include $(RTE_SDK)/mk/rte.vars.mk


CC=g++

# binary name
APP = slank

# all source are stored in SRCS-y
SRCS-y := main.cc

CFLAGS += $(WERROR_FLAGS)

# workaround for a gcc bug with noreturn attribute
# http://gcc.gnu.org/bugzilla/show_bug.cgi?id=12603
ifeq ($(CONFIG_RTE_TOOLCHAIN_GCC),y)
CFLAGS_main.o += -Wno-return-type
endif

CXXFLAGS += -std=c++11
CFLAGS += -O3 
CFLAGS += -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable
LDFLAGS += -lstdc++
# EXTRA_CFLAGS += -O3 -g -Wfatal-errors

include $(RTE_SDK)/mk/rte.extapp.mk
