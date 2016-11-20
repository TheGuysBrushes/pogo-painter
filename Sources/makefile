projet:  projet.o pipes.o affichages.o initialisations.o threads.o types.o
	gcc projet.o pipes.o affichages.o initialisations.o threads.o types.o -o projet -lncurses -m32 -pthread
	
projet1:  projet_pipes_tableau.o pipes.o affichages.o initialisations.o threads.o types.o
	gcc -m32 projet_pipes_tableau.o pipes.o affichages.o initialisations.o threads.o types.o -o projet1 -lncurses -pthread
	
projet.o: projet.c pipes.c
	gcc -m32 -c -Wall projet.c
	
projet_pipes_tableau.o: projet_pipes_tableau.c pipes.h affichages.h initialisations.h threads.h types.h
	gcc -m32 -c -Wall projet_pipes_tableau.c
	
pipes.o: pipes.c
	gcc -m32 -c -Wall pipes.c
	
affichages.o: affichages.c
	gcc -m32 -c -Wall affichages.c
	
initialisations.o: initialisations.c
	gcc -m32 -c -Wall initialisations.c

threads.o: threads.c
	gcc -m32 -c -Wall threads.c
		
types.o: types.c
	gcc -m32 -c -Wall types.c

clean:
	rm *.o
