
FLAG = -Wextra -Wall -std=c++0x -g -o

OBJECTS = ./obj/pawn.o ./obj/peice.o ./obj/elephant.o ./obj/king.o ./obj/queen.o ./obj/camel.o ./obj/horse.o ./obj/empty.o ./obj/entity.o ./obj/render.o ./obj/main.o

output:  $(OBJECTS)
	g++ $(OBJECTS) $(FLAG) game 


# user created files compilation........
./obj/main.o: main.cpp
	g++ -c main.cpp $(FLAG) ./obj/main.o

./obj/peice.o: peice.cpp peice.h
	g++ -c peice.cpp $(FLAG) ./obj/peice.o

./obj/pawn.o: pawn.cpp pawn.h
	g++ -c pawn.cpp $(FLAG) ./obj/pawn.o

./obj/elephant.o: elephant.cpp elephant.h
	g++ -c elephant.cpp $(FLAG) ./obj/elephant.o

./obj/king.o: king.cpp king.h
	g++ -c king.cpp $(FLAG) ./obj/king.o
	
./obj/camel.o: camel.cpp camel.h
	g++ -c camel.cpp $(FLAG) ./obj/camel.o

./obj/queen.o: queen.cpp queen.h
	g++ -c queen.cpp $(FLAG) ./obj/queen.o

./obj/horse.o: horse.cpp horse.h
	g++ -c horse.cpp $(FLAG) ./obj/horse.o

./obj/empty.o: empty.cpp empty.h
	g++ -c empty.cpp $(FLAG) ./obj/empty.o

# game engine related files.........
./obj/entity.o: ./engine/entity.cpp ./engine/entity.h 
	g++ -c ./engine/entity.cpp $(FLAG) ./obj/entity.o

./obj/render.o: ./engine/render.cpp ./engine/render.h
	g++ -c ./engine/render.cpp  $(FLAG) ./obj/render.o

clean:
	-@rm ./obj/*.o game
	
