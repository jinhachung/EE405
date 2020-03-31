all:
	make clean
	make local-reaction-timer
	make target-reaction-timer
	make local-test-getch
	make target-test-getch

clean:
	-rm reaction_timer reaction_timer_pc test_getch test_getch_pc

local-reaction-timer: reaction_timer.c getche.c
	gcc -g -o reaction_timer_pc reaction_timer.c getche.c
target-reaction-timer: reaction_timer.c getche.c
	arm-linux-gnueabihf-gcc -o reaction_timer reaction_timer.c getche.c 
local-test-getch: test_getch.c getche.c
	gcc -g -o test_getch_pc test_getch.c getche.c
target-test-getch: test_getch.c getche.c
	arm-linux-gnueabihf-gcc -o test_getch test_getch.c getche.c
