CC=gcc
CROSS_CC=arm-linux-gnueabihf-gcc

all: pc bone

pc: server.c client.c
	$(CC) server.c -o server_pc
	$(CC) client.c -o client_pc

	$(CC) talker.c -o talker_pc
	$(CC) listener.c -o listener_pc

bone: server.c client.c
	$(CROSS_CC) server.c -o server_bone
	$(CROSS_CC) client.c -o client_bone

	$(CROSS_CC) talker.c -o talker_bone
	$(CROSS_CC) listener.c -o listener_bone

clean:
	rm server_pc client_pc
	rm server_bone client_bone
	rm talker_pc listener_pc
	rm talker_bone listener_bone
