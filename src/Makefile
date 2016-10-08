EXEC = dreamflower_app
OBJS = dreamflower_app.o
<<<<<<< HEAD
libobjs := uart1.o uart_1_app.o
=======
libobjs := uart1.o
>>>>>>> 1ed53b67f2b8c838edf58073b746fc31b907fec3

all: $(EXEC)

$(EXEC): $(OBJS) $(libobjs)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(libobjs)

clean:
	rm -f rbcfg *.o $(EXEC)
