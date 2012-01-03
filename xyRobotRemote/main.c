#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <errno.h>
#include <string.h>

#include "serial.h"
#include "x11.h"
#include "util.h"

int eventLoop = 1; // Event loop expression

char toBeDrawn = 0;
unsigned char cameraBuffer[128][128];
char helpBuf[8][30];
char helpSize[8];
char statusBuf[2][60];
char statusSize[2];

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
	for (i = 0; i < 8; i++) {
		n = helpSize[i];
		text.nchars = n;
		text.chars = helpBuf[i];
		XDrawText(dsp, win, gc, 148, (i * 12) + 22, &text, 1);
	}
	for (i = 0; i < 2; i++) {
		n = statusSize[i];
		text.nchars = n;
		text.chars = statusBuf[i];
		XDrawText(dsp, win, gc, 10, (i * 12) + 160, &text, 1);
	}
}

void draw() {
	XClearWindow(dsp, win);
	XCopyArea(dsp, cameraMap, win, gc, 0, 0, 128, 128, 10, 10);
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
	while (s[n] != '\0') {
		n++;
	}
	for (i = 0; i <= n; i++) {
		helpBuf[line][i] = s[i];
	}
	helpSize[line] = n;
	toBeDrawn = 1;
}

void drawStatus(char *s, int line) {
	int i, n = 0;
	while (s[n] != '\0') {
		n++;
	}
	for (i = 0; i <= n; i++) {
		statusBuf[line][i] = s[i];
	}
	statusSize[line] = n;
	toBeDrawn = 1;
}

void randomizeCamBuf() {
	int i, j;
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 128; j++) {
			cameraBuffer[i][j] = rand() % 256;
		}
	}
	drawStatus("Randomized!", 1);
	drawCamMap();
}

void blackCamBuf() {
	int i, j;
	for (i = 0; i < 128; i++) {
		for (j = 0; j < 128; j++) {
			cameraBuffer[i][j] = 0;
		}
	}
	drawStatus("Pitch Black!", 1);
	drawCamMap();
}

void captureImage() {
	int x = 0, y = 0, t;
	char c = 'c';

	drawStatus("Capture...", 1);
	draw(); // Needs to happen now!

	while((t = serialWrite(&c, 1)) != 1) {
		if (t == -1) {
			drawStatus(strerror(errno), 1);
			toBeDrawn = 1;
			return;
		}
	} // Send 'c' command
	pauseExec(1); // Wait a second
	
	// Recieve
	startTimer();
	while (y < 128) {
		x = 0;
		while (x < 128) {
			while (serialRead(&cameraBuffer[x][y], 1) != 1) {
				if (getTimerDiff() >= 10000) { // Max. 10 seconds
					drawStatus("Robot doesn't answer!", 1);
					toBeDrawn = 1;
					return;
				}
			}
			x++;
		}
		y++;
	}
	drawStatus("Got picture!", 1);
	drawCamMap();
}

void tryConnect() {
	char c[1] = { '?' };
	char buf[42];
	int i = 0, t;

	buf[12] = '\0';

	drawStatus("Sending ping...", 1);
	draw();

	while((t = serialWrite(c, 1)) != 1) {
		if (t == -1) {
			drawStatus(strerror(errno), 1);
			toBeDrawn = 1;
			return;
		}
	} // Send ?

	drawStatus("Waiting for robot...", 1);
	draw();

	startTimer();
	do {
		t = serialRead((buf + i), 1); // Read a char
		if (i == -1) {
			drawStatus(strerror(errno), 1);
			toBeDrawn = 1;
			return;
		}
		if (getTimerDiff() >= 4000) {
			drawStatus("Robot doesn't answer!", 1);
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

	drawStatus(buf, 1);
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
	}
}

int main(int argc, char **argv){

	XEvent evt;

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
	drawHelp("p: Try contacting robot", 5);
	drawStatus("Status:", 0);
	drawStatus("Ready...", 1);

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
