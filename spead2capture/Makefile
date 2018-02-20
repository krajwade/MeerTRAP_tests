CC = g++
SPEAD2_INC_FLAGS = -I /usr/local/include#define if needed (as -I[...])
ifeq ($(OPT_DEBUG), yes)
CFLAGS = -g -pedantic -Wall -std=gnu++11 $(SPEAD2_INC_FLAGS)
else
CFLAGS = -O3 -pedantic -Wall -std=gnu++11 $(SPEAD2_INC_FLAGS) 
endif

# linker flags
SPEAD2_LIB_FLAGS = -L /usr/local/lib# define if not in $PATH (as -L[...])
LFLAGS = $(SPEAD2_LIB_FLAGS) -lspead2 -lboost_system -lboost_program_options -pthread -lpthread 

 # directories
BINDIR =./bin


# command definitions
DELCMD = rm

all: fbf_send generic_recv

generic_recv: generic_recv.cpp
	$(CC) $< $(CFLAGS) $(LFLAGS) -o $(BINDIR)/$@ 

fbf_send: fbf_send.cpp
	$(CC) $< $(CFLAGS) $(LFLAGS) -o $(BINDIR)/$@ 


install:
	@echo Nothing to install.

clean:
	@echo Nothing to clean.
