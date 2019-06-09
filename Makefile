ASM         = nasm
ASMFLAGS    = -felf64 -g -Isrc/ 
LINKER      = ld

all: bin/forthress
	
bin/forthress: obj/forthress.o
	mkdir -p bin 
	$(LINKER) -o bin/forthress $(LINKERFLAGS) -o bin/forthress obj/forthress.o $(LIBS)

obj/forthress.o: src/forthress.asm src/macro.inc src/kernel.inc src/util-words.inc src/interpreter.inc src/lib.inc
	mkdir -p obj
	$(ASM) $(ASMFLAGS) src/forthress.asm -o obj/forthress.o

clean: 
	rm -rf bin obj

