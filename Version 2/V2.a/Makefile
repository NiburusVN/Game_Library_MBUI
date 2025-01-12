CC ?= gcc
CFLAGS ?= $(INC_FLAGS) -MMD -MP -O3 -DNDEBUG -Wall -Wextra -pedantic -std=c11 -Wno-unused-parameter

EXEC_M = V2.a #main
EXEC_S = Cfichier/ajouterServ Cfichier/lancerServ Cfichier/listerServ Cfichier/simulerServ Cfichier/supprimerServ Cfichier/testerServ #les servers

ALL = $(EXEC_M) $(EXEC_S)

all: $(ALL)

# fichiers executables
V2.a: main.o Cfichier/utils.o
	$(CC) -o $@ $^ $(CFLAGS)

Cfichier/ajouterServ: Cfichier/ajouterServ.o 
	$(CC) -o $@ $^ $(CFLAGS)

Cfichier/lancerServ: Cfichier/lancerServ.o 
	$(CC) -o $@ $^ $(CFLAGS)

Cfichier/listerServ: Cfichier/listerServ.o
	$(CC) -o $@ $^ $(CFLAGS)

Cfichier/simulerServ: Cfichier/simulerServ.o
	$(CC) -o $@ $^ $(CFLAGS)

Cfichier/supprimerServ: Cfichier/supprimerServ.o 
	$(CC) -o $@ $^ $(CFLAGS)

Cfichier/testerServ: Cfichier/testerServ.o 
	$(CC) -o $@ $^ $(CFLAGS)


# fichiers .o
utils.o: Cfichier/utils.c
	$(CC) $(CFLAGS) -c Cfichier/utils.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

ajouterServ.o: Cfichier/ajouterServ.c
	$(CC) $(CFLAGS) -c Cfichier/ajouterServ.c

lancerServ.o: Cfichier/lancerServ.c
	$(CC) $(CFLAGS) -c Cfichier/lancerServ.c

listerServ.o: Cfichier/listerServ.c
	$(CC) $(CFLAGS) -c Cfichier/listerServ.c

simulerServ.o: Cfichier/simulerServ.c
	$(CC) $(CFLAGS) -c Cfichier/simulerServ.c

supprimerServ.o: Cfichier/supprimerServ.c
	$(CC) $(CFLAGS) -c Cfichier/supprimerServ.c

testerServ.o: Cfichier/testerServ.c
	$(CC) $(CFLAGS) -c Cfichier/testerServ.c


clean:
	rm *.o *.d Cfichier/*o Cfichier/*d 



