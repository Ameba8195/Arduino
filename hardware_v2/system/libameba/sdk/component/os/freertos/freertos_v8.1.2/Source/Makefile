
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

MODULE_IFLAGS = -I./include


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = tasks.o list.o croutine.o queue.o timers.o event_groups.o 
ifeq ($(CONFIG_RELEASE_BUILD),y)
	OBJS = 
else
endif


#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
#all: CORE_TARGETS COPY_RAM_OBJS
all: CORE_TARGETS COPY_RAM_OBJS
	make -C portable/MemMang all
	make -C portable/GCC/ARM_CM4F all
	

	
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)


#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C portable/MemMang clean
	make -C portable/GCC/ARM_CM4F clean
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d
	
-include $(DEPS)
