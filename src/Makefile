CPPFLAGS ?= -pedantic-errors -Wall
CFLAGS   ?= -pipe

CPPFLAGS += -std=c99
LDFLAGS  += -lhiredis

recommendify: recommendify.c cosine.c jaccard.c output.c

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^

.c:
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $>
