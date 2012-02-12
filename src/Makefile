recommendify = recommendify.c cosine.c jaccard.c output.c

recommendify: $(recommendify)
	$(CC) -std=c99 $(CPPFLAGS) $(CFLAGS) -o $@ $(recommendify) $(LDFLAGS) -lhiredis
