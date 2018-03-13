CC := gcc
CFLAGS := -Wall -g -Os



OBJS = ftclient.o 

all: ftclient

ftclient: $(OBJS)
	@$(CC) -o ftclient $(CFLAGS) $(OBJS)

$(OBJS) : %.o: %.c 
	@$(CC) -c $(CFLAGS) $< -o $@

.PHONY:
clean:
	@rm -f *.o ftclient
	@echo Done cleaning
