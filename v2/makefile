
##
## PIN tools
##

##############################################################
#
## User-specific configuration
#
###############################################################

#
## 1. Change PIN_ROOT to point to the top-level directory where
#    Pin was installed. This can also be set on the command line,
#    #    or as an environment variable.
#
PIN_ROOT ?= /home/cs246/pin
#TARGET = ia32

##############################################################
#
## set up and include *.config files
#
###############################################################

KIT = 1
COMPILER ?= gcc
TARGET_OS = linux


ifeq ($(wildcard $(PIN_ROOT)/source/tools/Config/unix.vars),)
    $(error Set PIN_ROOT to a valid Pin installation)
endif

include $(PIN_ROOT)/source/tools/Config/unix.vars
include $(PIN_ROOT)/source/tools/Config/makefile.unix.config

TOOL_ROOTS = main
TOOLS = $(TOOL_ROOTS:%=$(OBJDIR)%$(PINTOOL_SUFFIX))
TOOL_OBJS = $(OBJDIR)$(TOOL_ROOTS).o $(OBJDIR)cache.o $(OBJDIR)percepTable.o


## Additional flags
CXXFLAGS += -std=c++11

## build 
all:  tools  
tools: $(OBJDIR) $(TOOLS)

## build rules

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)%.o : %.cpp
	$(CXX) ${COPT} $(CXXFLAGS) $(TOOL_CXXFLAGS) ${COMP_OBJ}$@ $<

$(TOOLS): $(TOOL_OBJS) $(PIN_LIBNAMES)
	${LINKER} $(TOOL_LDFLAGS) $(LINK_DEBUG) ${LINK_EXE}$@ $^ ${TOOL_LPATHS} $(TOOL_LIBS)

## cleaning
clean:
	-rm -rf $(OBJDIR) *.out pin.log
