# Common variables
BUILD ?= release
CFLAGS = -std=gnu99

# Client side variables 
CC ?= gcc
CLIENT_TARGET = ledcon-client
CLIENT_SRCDIR = client
CLIENT_BINDIR = client/bin
CLIENT_OBJDIR = client/obj
CLIENT_SOURCES = $(CLIENT_SRCDIR)/client_main.c	\
				 $(CLIENT_SRCDIR)/client_core.c
CLIENT_OBJECTS = $(patsubst $(CLIENT_SRCDIR)/%.c, $(CLIENT_OBJDIR)/%.o, $(CLIENT_SOURCES))

# Server side variables 
CROSS_COMPILE ?=
SERVER_TARGET = ledcon-server
SERVER_SRCDIR = server
SERVER_BINDIR = server/bin
SERVER_OBJDIR = server/obj
SERVER_SOURCES = $(SERVER_SRCDIR)/server_main.c	\
				 $(SERVER_SRCDIR)/server_core.c
SERVER_OBJECTS = $(patsubst $(SERVER_SRCDIR)/%.c, $(SERVER_OBJDIR)/%.o, $(SERVER_SOURCES))

# Add debug or optimization flags based on build mode
ifeq ($(BUILD), debug)
	CFLAGS += -Wall -g
else
	ifeq ($(BUILD), release)
		CFLAGS += -O2
	endif
endif

all: $(CLIENT_TARGET) $(SERVER_TARGET)

# Building the client part
$(CLIENT_TARGET): $(CLIENT_OBJECTS)
	@mkdir -p $(CLIENT_BINDIR)
	$(CC) $(CFLAGS) $^ -o $(CLIENT_BINDIR)/$@

$(CLIENT_OBJDIR)/%.o: $(CLIENT_SRCDIR)/%.c
	@mkdir -p $(CLIENT_OBJDIR)
	$(CC) -c $(CFLAGS) $< -o $@

# Building the server part
$(SERVER_TARGET): $(SERVER_OBJECTS)
	mkdir -p $(SERVER_BINDIR)
	$(CROSS_COMPILE) $(CFLAGS) $^ -o $(SERVER_BINDIR)/$@

$(SERVER_OBJDIR)/%.o: $(SERVER_SRCDIR)/%.c
	mkdir -p $(SERVER_OBJDIR)
	$(CROSS_COMPILE) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(CLIENT_OBJDIR) $(SERVER_OBJDIR)

clean-client:
	rm -rf $(CLIENT_OBJDIR)

clean-server:
	rm -rf $(SERVER_OBJDIR)

distclean:
	rm -rf $(CLIENT_OBJDIR) $(CLIENT_BINDIR) $(SERVER_OBJDIR)  $(SERVER_BINDIR)

distclean-client:
	rm -rf $(CLIENT_OBJDIR) $(CLIENT_BINDIR)

distclean-server:
	rm -rf $(SERVER_OBJDIR)  $(SERVER_BINDIR)

help:
	@echo "CROSS_COMPILE - full name of the cross compiler"
	@echo "make CROSS_COMPILE=<your-cross-compiler> - Build the server and client"
	@echo "make CROSS_COMPILE=<your-cross-compiler> ledcon-client - Build only the client"
	@echo "make CROSS_COMPILE=<your-cross-compiler> ledcon-server - Build only the server"
	@echo ""
	@echo "make CROSS_COMPILE=<your-cross-compiler> BUILD=debug - Build with debug flags"
	@echo "make CROSS_COMPILE=<your-cross-compiler> BUILD=release - Build with optimization flags"
	@echo ""
	@echo "make clean - Remove all temporary files for the server and client"
	@echo "make clean-client - Remove all temporary files for the client only"
	@echo "make clean-server - Remove all temporary files for the server only"
	@echo ""
	@echo "make distclean - Remove all generated files for the server and client"
	@echo "make distclean-client - Remove all generated files for the client only"
	@echo "make distclean-server - Remove all generated files for the server only"

.PHONY: all clean clean-client clean-server distclean distclean-client distclean-server help