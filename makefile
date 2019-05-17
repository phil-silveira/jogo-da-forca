all: s.exe c.exe

s.exe: JogoForcaServ.c
	gcc JogoForcaServ.c -o s.exe

c.exe: JogoForcaCli.c
	gcc JogoForcaCli.c -o c.exe

clean:
	rm *.exe
