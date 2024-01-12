build_all: clean server client debug full

client:
	gcc -Wall -o client MTP_Client.c

server:
	gcc -Wall -o server MTP_Server.c -lpthread

clean:
	-rm server client bin/dbg/clientDbg bin/dbg/serverDbg fullClient

rebuild: clean server client debug build_ful

release:
	@mkdir -p bin/rel
	gcc -Wall -o bin/dbg/clientDbg MTP_Client.c
	gcc -Wall -o bin/dbg/serverDbg MTP_Server.c -lpthread

debug:
	@mkdir -p bin/dbg
	gcc -g -O0 -Wall -o bin/dbg/clientDbg MTP_Client.c
	gcc -g -O0 -Wall -o bin/dbg/serverDbg MTP_Server.c -lpthread

full: server
	gcc -DFULL -Wall -o fullClient MTP_Client.c

demo: server client

