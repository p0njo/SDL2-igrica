all:
	g++ -Isrc/Include -Lsrc/lib -o main main.cpp Player.cpp ArenaGate.cpp Teleporter.cpp Enemy.cpp hub.cpp  -lmingw32 -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2 -static-libstdc++ -static-libgcc