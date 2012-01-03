#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "util.h"

char colorBuffer[8];
clock_t timer;

void grayToColor(unsigned char gray) {
	sprintf(colorBuffer, "#%X%X%X", gray, gray, gray);
	if (colorBuffer[4] == '\0') {
		colorBuffer[7] = colorBuffer[4];
		colorBuffer[6] = colorBuffer[3];
		colorBuffer[5] = '0';
		colorBuffer[4] = colorBuffer[2];
		colorBuffer[3] = '0';
		colorBuffer[2] = colorBuffer[1];
		colorBuffer[1] = '0';
	}
}

void startTimer() {
	timer = clock();
}

int getTimerDiff() {
#ifdef CLK_PER_SEC
	int cPerMilli = CLK_PER_SEC / 1000;
#else
	int cPerMilli = CLOCKS_PER_SEC / 1000;
#endif
	clock_t now = clock();
	now -= timer;
	return (now / cPerMilli);
}

void pauseExec(int seconds) {
	sleep(seconds);
}
