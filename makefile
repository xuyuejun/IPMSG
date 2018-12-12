obj=main.o users.o network_service.o user_interface.o mytcp.o
CC=gcc
CFLAGS= -Wall -O2


ipmsg:$(obj)
	$(CC) -o $@ $^ -lpthread

%O:%C %h
	$(CC) -c -o $@ $^

clean:
	@rm $(obj) -rf main -rf
