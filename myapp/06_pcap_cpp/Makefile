


CC = g++
LIBS = -L$(RTE_SDK)/x86_64-native-linuxapp-gcc/lib -ldpdk -lpthread -ldl
INCLUDES =  -I$(RTE_SDK)/x86_64-native-linuxapp-gcc/include
INCLUDE_FILES = -include $(RTE_SDK)/x86_64-native-linuxapp-gcc/include/rte_config.h



CCFLAGS = -g -D__STDC_LIMIT_MACROS  # C++ compiler flags (-g -O2 -Wall)
#
CCFLAGS += -DRTE_MAX_LCORE=64 
			# -DRTE_PKTMBUF_HEADROOM=128 \
			# -DRTE_MAX_ETHPORTS=32

# CCFLAGS += -DRTE_MACHINE_CPUFLAG_SSE \
#                   -DRTE_MACHINE_CPUFLAG_SSE2 \
#                   -DRTE_MACHINE_CPUFLAG_SSE3 \
#                   -DRTE_MACHINE_CPUFLAG_SSSE3 \
#                   -DRTE_COMPILE_TIME_CPUFLAGS=RTE_CPUFLAG_SSE,RTE_CPUFLAG_SSE2,RTE_CPUFLAG_SSE3,RTE_CPUFLAG_SSSE3

SRC = main.cc

app:
	$(CC) $(CCFLAGS) $(SRC) -o $@ $(LIBS) $(INCLUDES) $(INCLUDE_FILES)


clean:
	rm app
