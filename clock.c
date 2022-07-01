#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <termios.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/ioctl.h>

// used to tell the program when to quit incase one of the following key is pressed:
// q, ctrl + c, or ctrl + d
volatile int quit = 0;
void set_raw(struct termios* old_tio) {
	struct termios new_tio;
	tcgetattr(0, old_tio);
	new_tio = *old_tio;
	new_tio.c_lflag = 0;
	tcsetattr(0, TCSANOW, &new_tio);
	cfmakeraw(&new_tio);
}

void set_tio(struct termios* tio) {
	tcsetattr(0, TCSANOW, tio);
}

void draw_clock() {
	time_t tloc = 0;
	struct tm* ltime = NULL;
	char* time_str = calloc(12, sizeof(char));
	tloc = time(NULL);
	ltime = localtime(&tloc);
	int right_col = 58;
	strftime(time_str, 12, "%I:%M:%S %p", ltime);
	// print the top of the clock
	printf("\t\t\t\t\t\t\t  \033[33m________________________________\n\033[0m");
	for(int i=0;i<3;i++) {
		for(int j=0;j<7;j++) { printf("\t"); }
		printf("  |");
		for(int j=0;j<30;j++) { printf(" "); }
		printf("|\n");
	}
	// print the rows for the clock frame
	printf("\t\t\t\t\t\t\t  |");
	for(int i=0;i<8;i++) { printf(" "); }
	// print the row of the clock that displays the time
	printf("\033[31m%s\033[0m", time_str);
	for(int i=0;i<11;i++) { printf(" "); }
	printf("|\n");
	// print the rest of the rows for the clock frame
	for(int i=0;i<3;i++) {
		for(int j=0;j<7;j++) { printf("\t"); }
		printf("  |");
		for(int j=0;j<30;j++) { printf(" "); }
		printf("|\n");
	}
	// print the last part of the frame for the clock
	printf("\t\t\t\t\t\t\t  \033[33m--------------------------------\n\033[0m");
	printf("\t\t\t\t\t\t\tPress \033[34mq\033[0m, \033[34mctrl + c\033[0m, or \033[34mctrl\033[0m + \033[34md\033[0m to quit\n\033[6A");
	fflush(stdout);
	while(1) {
		tloc = time(NULL);
		ltime = localtime(&tloc);
		strftime(time_str, 12, "%I:%M:%S %p", ltime);
		printf("\033[%dC|", right_col);
		for(int i=0;i<8;i++) { printf(" "); }
		printf("\033[31m%s\033[0m", time_str);
      		for(int i=0;i<11;i++) { printf(" "); }
		printf("|\033[%dD", (84) + 14);
		fflush(stdout);
		if(quit) {
			break;
		}
	}
	printf("\033[6B");
	free(time_str); time_str = NULL;
}

int main() {
	struct termios old_tio;
	char c = 0;
	int esc = 0;
	set_raw(&old_tio);
	pthread_t thread;
	pthread_create(&thread, NULL, (void*)draw_clock, NULL);
	/* TODO: make clock responsive (resizeable)
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	printf("lines: %d, columns: %d\n", w.ws_row, w.ws_col);
	*/
	while((c = fgetc(stdin))) {
		if(c == 'q' || c == 3 || c == 4) {
			quit = 1;
			break;
		}
	}
	pthread_join(thread, NULL);
	set_tio(&old_tio);
	return 0;
}
