
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

MODULE_IFLAGS = 


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = port.o

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CORE_TARGETS COPY_RAM_OBJS

	
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

clean:
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d

-include $(DEPS)
