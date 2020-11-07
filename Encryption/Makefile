all: vencrypt vdecrypt scrypt sbencrypt sbdecrypt

vencrypt: vencrypt.c
	$(CC) $(CCFLAGS) -g -lm -o vencrypt vencrypt.c

vdecrypt: vdecrypt.c
	$(CC) $(CCFLAGS) -g -lm -o vdecrypt vdecrypt.c

scrypt: scrypt.c
	$(CC) $(CCFLAGS) -g -lm -o scrypt scrypt.c
	
sbencrypt: sbencrypt.c
	$(CC) $(CCFLAGS) -g -lm -o sbencrypt sbencrypt.c

sbdecrypt: sbdecrypt.c
	$(CC) $(CCFLAGS) -g -lm -o sbdecrypt sbdecrypt.c	

clean:
	rm -rf vencrypt
	rm -rf vdecrypt
	rm -rf scrypt
	rm -rf sbencrypt
	rm -rf sbdecrypt
