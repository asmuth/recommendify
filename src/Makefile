recommendify = recommendify.c cosine.c jaccard.c output.c

recommendify: $(recommendify)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(recommendify) $(LDFLAGS) -lhiredis
