EXEC = dreamflower_app
OBJS = dreamflower_app.o
libobjs := uart1.o

all: $(EXEC)

$(EXEC): $(OBJS) $(libobjs)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(libobjs)

clean:
	rm -f rbcfg *.o $(EXEC)
