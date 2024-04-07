<<<<<<< HEAD
all:
	g++ -Isrc/Include -Lsrc/lib -o main main.cpp Player.cpp ArenaGate.cpp Teleporter.cpp Enemy.cpp hub.cpp  -lmingw32 -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2 -static-libstdc++ -static-libgcc
=======
all:
	g++ -Isrc/Include -Lsrc/lib -o main main.cpp Player.cpp ArenaGate.cpp Teleporter.cpp -lmingw32 -lSDL2main -lSDL2_image -lSDL2 -static-libstdc++ -static-libgcc
>>>>>>> d04f3e9a6bc4a6ecccf3f1dab7ae157ba673a0ea
