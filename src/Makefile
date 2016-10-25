EXEC = dreamflower_app
OBJS = dreamflower_app.o
libobjs := uart1.o uart_1_app.o gpio.o Daemon.o fdebug.o calendar.o tcpdump.o thread.o

#-增加库变异的方法先保留,这个地方不去深究
#LIBOBJSA = tcpdump/tcpdump.a

all: $(EXEC)

#tcpdump/tcpdump.a:
#	cd tcpdump && $(MAKE) tcpdump.a

$(EXEC): $(OBJS) $(libobjs)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(libobjs) -lpcap -lpthread

clean:
	rm -f rbcfg *.o $(EXEC)
