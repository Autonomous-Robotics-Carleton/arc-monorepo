# Our modules (ADR-0037), joined to the firmware build by one line in
# bldc/make/fw.mk. src/ is plain C, also built and tested on the host
# (CMakeLists.txt); port/ is the firmware-only glue to ChibiOS and the board.
ARCDIR = ../arc

CSRC += $(wildcard $(ARCDIR)/src/*.c) $(wildcard $(ARCDIR)/port/*.c)

INCDIR += $(ARCDIR)/include
