#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include "serial.h"
#include "x11.h"
#include "util.h"

int eventLoop = 1; // Event loop expression

char toBeDrawn = 0;
unsigned char cameraBuffer[128][128];
char helpBuf[12][127];
char helpSize[12];
char statusBuf[8][127];
char statusSize[8];
int nextStatusLine = 1;
char stringBuffer[127];
unsigned char cameraSettings[9] = { 0x7F,	// Z & Offset
									0x02,	// N & VH & Gain
									0x00,	// Exposure
									0x3C,	// Exposure small
									0x01,	// P
									0x00,	// M
									0x01,	// X
									0x04 };	// E & V

void drawStat(char *s, int line);
void changeGain(int dir); // 0: Reduce
void changeExposureS(int dir);
void changeExposure(int dir);

void initBuffs() {
	int i, j;
	for (i = 0; i < 8; i++) {
		helpSize[i] = 0;
		for (j = 0; j < 30; j++) {
			helpBuf[i][j] = '\0';
		}
	}
	for (i = 0; i < 2; i++) {
		statusSize[i] = 0;
		for (j = 0; j < 60; j++) {
			statusBuf[i][j] = '\0';
		}
	}
}

void drawText() {
	XTextItem text;
	int i, n;
	text.font = font;
	for (i = 0; i < 11; i++) {
		n = helpSize[i];
		text.nchars = n;
		text.chars = helpBuf[i];
		XDrawText(dsp, win, gc, 148, (i * 12) + 22, &text, 1);
	}
	for (i = 0; i < 8; i++) {
		if (statusBuf[i][0] == '\0') {
			break; // If we encounter an empty string, stop
		}
		n = statusSize[i];
		text.nchars = n;
		text.chars = statusBuf[i];
		XDrawText(dsp, win, gc, 10, (i * 12) + 160, &text, 1);
	}
}

void draw() {
	XClearWindow(dsp, win);
	XCopyArea(dsp, cameraMap, win, gc, 0, 0, 128, 128, 10, 10);
	drawStat("Status:", 0);
	drawText();
	XFlush(dsp);
	toBeDrawn = 0;
}

void drawCamMap() {
	int i, j;
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 128; j++) {
			grayToColor(cameraBuffer[i][j]);
			XParseColor(dsp, colormap, colorBuffer, &camColor);
			XAllocColor(dsp, colormap, &camColor);
			XSetForeground(dsp, gc, camColor.pixel);
			XDrawPoint(dsp, cameraMap, gc, i, j);
		}
	}
	toBeDrawn = 1;
}

void drawHelp(char *s, int line) {
	int i, n = 0;

	sprintf(helpBuf[line], "%s", s);
	helpSize[line] = strlen(s);
	toBeDrawn = 1;
}

void moveStatus(int src, int dest) {
	int i;
	sprintf(statusBuf[dest], "%s", statusBuf[src]);
	statusSize[dest] = statusSize[src];
}

void drawStatus(char *s) {
	drawStat(s, nextStatusLine);
	if (nextStatusLine < 7) {
		nextStatusLine++;
	}
}

void drawStat(char *s, int line) {
	int i, n = 0;

	if ((statusBuf[7][0] != '\0') && (line == 7)) {
		// We are at the last line, and it is already filled
		// So we scroll
		for (i = 0; i < 7; i++) {
			moveStatus(i + 1, i);
		}
	}
	

	sprintf(statusBuf[line], "%s", s);
	statusSize[line] = strlen(s);
	toBeDrawn = 1;
}

void randomizeCamBuf() {
	int i, j;
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 128; j++) {
			cameraBuffer[i][j] = rand() % 256;
		}
	}
	drawStatus("Randomized!");
	drawCamMap();
}

void blackCamBuf() {
	int i, j;
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 128; j++) {
			cameraBuffer[i][j] = 0;
		}
	}
	drawStatus("Pitch Black!");
	drawCamMap();
}

void captureImage() {
	int x = 0, y = 0, t, i;
	char c = 'c';

	drawStatus("Capture...");
	draw(); // Needs to happen now!

	while((t = serialWrite(&c, 1)) != 1) {
		if (t == -1) {
			drawStatus(strerror(errno));
			toBeDrawn = 1;
			return;
		}
	} // Send 'c' command
	// Send registers
	i = 0; // Num of bytes transmitted
	while (i < 8) {
		t = serialWrite((cameraSettings + i), (8 - i));
		if (t == -1) {
			drawStatus(strerror(errno));
			toBeDrawn = 1;
			return;
		} else {
			i += t;
		}
	}
	pauseExec(1); // Wait a second
	
	// Recieve
	startTimer();
	while (y < 128) {
		x = 0;
		while (x < 128) {
			while (serialRead(&cameraBuffer[x][y], 1) != 1) {
				if (getTimerDiff() >= 10000) { // Max. 10 seconds
					drawStatus("Robot doesn't answer!");
					toBeDrawn = 1;
					return;
				}
			}
			x++;
		}
		y++;
	}
	drawStatus("Got picture!");
	drawCamMap();
}

void tryConnect() {
	char c[1] = { '?' };
	char buf[42];
	int i = 0, t;

	buf[12] = '\0';

	drawStatus("Sending ping...");
	draw();

	while((t = serialWrite(c, 1)) != 1) {
		if (t == -1) {
			drawStatus(strerror(errno));
			toBeDrawn = 1;
			return;
		}
	} // Send ?

	drawStatus("Waiting for robot...");
	draw();

	startTimer();
	do {
		t = serialRead((buf + i), 1); // Read a char
		if (i == -1) {
			drawStatus(strerror(errno));
			toBeDrawn = 1;
			return;
		}
		if (getTimerDiff() >= 4000) {
			drawStatus("Robot doesn't answer!");
			toBeDrawn = 1;
			return;
		}
		if (t == 1) {
			// We actually got a char
			if (buf[i] == '\n') {
				// It's a newline...
				buf[i] = '\0';
				break;
			} else {
				// On to the next
				i++;
			}
		}
	} while (1);

	drawStatus(buf);
	toBeDrawn = 1;

	while(serialRead(buf, 12) != 0); // Flush input, probably garbage
}

void keyReact(KeySym key) {
	switch (key) {
		case XK_Escape:
		case XK_q:
			eventLoop = 0;
			break;
		case XK_r:
			randomizeCamBuf();
			break;
		case XK_b:
			blackCamBuf();
			break;
		case XK_c:
			captureImage();
			break;
		case XK_p:
			tryConnect();
			break;
		case XK_g:
			changeGain(0);
			break;
		case XK_h:
			changeGain(1);
			break;
		case XK_j:
			changeExposureS(0);
			break;
		case XK_k:
			changeExposureS(1);
			break;
		case XK_n:
			changeExposure(0);
			break;
		case XK_m:
			changeExposure(1);
			break;
	}
}

void changeGain(int dir) {
	unsigned char gain = cameraSettings[1] & 0x0F;
	if (dir == 0) {
		// Decrease
		if (gain > 0) {
			gain--;
			sprintf(stringBuffer, "Gain: %d", gain);
			drawStatus(stringBuffer);
		} else {
			drawStatus("Lowest gain possible: 0");
		}
	} else {
		// Increase
		if (gain < 0x0F) {
			gain++;
			sprintf(stringBuffer, "Gain: %d", gain);
			drawStatus(stringBuffer);
		} else {
			drawStatus("Highest gain possible: 15");
		}
	}
	cameraSettings[1] = (cameraSettings[1] & 0xF0) | (gain);
}

void changeExposureS(int dir) {
	unsigned char exp = cameraSettings[3];
	if (dir == 0) {
		// Decrease
		if (exp >= 10) {
			exp -= 10;
			sprintf(stringBuffer, "ExposureSmall: %d", exp);
			drawStatus(stringBuffer);
		} else {
			drawStatus("Lowest ExpSm possible: 0");
		}
	} else {
		// Increase
		if (exp <= 245) {
			exp += 10;
			sprintf(stringBuffer, "ExposureSmall: %d", exp);
			drawStatus(stringBuffer);
		} else {
			drawStatus("Highest ExpSm possible: 255");
		}
	}
	cameraSettings[3] = exp;
}

void changeExposure(int dir) {
	unsigned char exp = cameraSettings[2];
	if (dir == 0) {
		// Decrease
		if (exp >= 10) {
			exp -= 10;
			sprintf(stringBuffer, "Exposure: %d", exp);
			drawStatus(stringBuffer);
		} else {
			drawStatus("Lowest Exp possible: 0");
		}
	} else {
		// Increase
		if (exp <= 245) {
			exp += 10;
			sprintf(stringBuffer, "Exposure: %d", exp);
			drawStatus(stringBuffer);
		} else {
			drawStatus("Highest Exp possible: 255");
		}
	}
	cameraSettings[2] = exp;
}

void intHandler(int dummy) {
	xClose();
	serialClose();
}

int main(int argc, char **argv){

	XEvent evt;

	signal(SIGINT, intHandler);
	signal(SIGQUIT, intHandler);

	if (argc <= 1) {
		printf("Usage:\n%s /path/to/serial\n", argv[0]);
		return 1;
	} else {
		if (serialOpen(argv[1]) == -1) {
			printf("Error opening serial port!\n");
			return 1;
		}
	}

	if (xInit() != 0) {
		printf("Error while creating Window\n");
		return 1;
	}

	initBuffs();

	XSelectInput( dsp, win, ExposureMask | KeyReleaseMask | ButtonReleaseMask | StructureNotifyMask );

	Atom wmDelete = XInternAtom(dsp, "WM_DELETE_WINDOW", True); // React to window managers...
	XSetWMProtocols(dsp, win, &wmDelete, 1); // ...close action

	drawCamMap();
	
	drawHelp("xyRobotRemote", 0);
	drawHelp("q: Quit", 1);
	drawHelp("r: Randomize cam buffer", 2);
	drawHelp("b: Black cam buffer", 3);
	drawHelp("c: Capture image", 4);
	drawHelp("p: Probe for robot", 5);
	drawHelp("g: Reduce Gain", 6);
	drawHelp("h: Increase Gain", 7);
	drawHelp("j: Reduce Exposure Small", 8);
	drawHelp("k: Increase Exposure Small", 9);
	drawHelp("n: Reduce Exposure", 10);
	drawHelp("m: Increase Exposure", 11);

	drawStatus("Ready...");

	while (eventLoop) {
		XNextEvent(dsp, &evt);
		switch (evt.type) {
			case Expose:
				if (evt.xexpose.count == 0)
					toBeDrawn = 1;
				break;
			case ClientMessage: // User exited...
				eventLoop = 0;
				break;
			case ButtonRelease:
				
				break;
			case KeyRelease:
				keyReact(XLookupKeysym(&evt.xkey, 0));
				break;
			case ConfigureNotify:
				if (evt.xconfigure.width != WIDTH || evt.xconfigure.height != HEIGHT)
					XResizeWindow(dsp, win, WIDTH, HEIGHT);
				break;
		}
		if (toBeDrawn != 0) {
			draw();
			toBeDrawn = 0;
		}
	}

	xClose();
	serialClose();

	return 0;
}
