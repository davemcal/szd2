all: szd

szd: main.o graphics.o game_state.o level_state.o
	/usr/lib/gcc-snapshot/bin/g++ main.o graphics.o game_state.o level_state.o -o szd -lGLEW -lSDL -lSDL_image

main.o: main.cpp common.h game_state.h graphics2.h level_state.h
	/usr/lib/gcc-snapshot/bin/g++ -c main.cpp graphics2.cpp -std=c++0x

graphics.o: graphics2.cpp graphics2.h
	/usr/lib/gcc-snapshot/bin/g++ -c graphics2.cpp -std=c++0x

game_state.o: game_state.cpp game_state.h common.h
	/usr/lib/gcc-snapshot/bin/g++ -c game_state.cpp -std=c++0x

level_state.o: level_state.cpp common.h game_state.h iterator_2d.h level_state.h graphics2.h
	/usr/lib/gcc-snapshot/bin/g++ -c level_state.cpp -std=c++0x

clean:
	rm -rf *o hello
