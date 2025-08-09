CC=clang
SRCDIR=src
INCDIR=include
BUILDDIR=build
OBJDIR=build/obj
DEPDIR=build/deps
OPT=-O2
CFLAGS=-std=c99 -Wall -Wextra -I$(INCDIR) -pipe -pedantic \
       -D_FORTIFY_SOURCE -D_GNU_SOURCE $(OPT) \
       -fstack-protector-all -fPIE -fPIC -g -MMD -MF $(DEPDIR)/$(@F).d
LDFLAGS=-pie

SERVER=$(BUILDDIR)/server
CLIENT=$(BUILDDIR)/client

OBJS=$(OBJDIR)/server.o $(OBJDIR)/client.o

all: $(SERVER) $(CLIENT)

$(SERVER): $(OBJDIR)/server.o | $(BUILDDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

$(CLIENT): $(OBJDIR)/client.o | $(BUILDDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR) $(DEPDIR)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(wildcard $(DEPDIR)/*.d)

server:
	@$(SERVER)

client:
	@$(CLIENT)

$(BUILDDIR) $(OBJDIR) $(DEPDIR):
	mkdir -p $@

clean:
	rm -rf $(BUILDDIR)

.PHONY: all server client clean
