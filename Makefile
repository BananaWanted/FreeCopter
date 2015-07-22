SRC := $(wildcard src/*.cpp)
TAR = $(patsubst src/%.cpp,dest/%.o,$(SRC))

.PHONY: all clean

#dest/a: src/gpio_mem_dump.c
#	gcc -std=c11 $< -o $@

all:
	@if [ ! -d ./dest ]; then \
	   mkdir ./dest ; \
	fi
	
	@for file in src/*.cpp; do \
		export objfile="$${file//src\//dest\/}";\
		export objfile="$${objfile//.cpp/.o}";\
		echo "$$objfile";\
		gcc -Wall -Wno-write-strings -Wno-sign-compare -std=c++11 -c $$file -pthread -lrt -o $$objfile;\
	done

	@gcc -Wall -Wno-write-strings -Wno-sign-compare -std=c++11 dest/*.o -pthread -lrt -lstdc++ -o dest/fc

dest/%.o: src/%.cpp
	echo $<
	echo $@
	g++ -Wall -Wno-sign-compare -std=c++11 -c $< -o $@

clean:
	rm -rf dest/