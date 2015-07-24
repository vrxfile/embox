INC = ./include
LIB = ./libs

%.o : %.s
	mc-as $< --include-path=$(INC) -g --arch=MCp042R100102  --output=$@

%.o : %.c
	mc-lcc -g -target=mcp-r1 -Wa--arch=MCp042R100102  -c $< -I$(INC)  -o $@

uart.imagebin : crt0.o uart.o 
	mc-ld $^ --library-path=$(LIB) --library=uart --output=$@
clean : 
	rm -f *.imagebin *.o
