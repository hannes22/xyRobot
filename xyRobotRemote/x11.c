#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#include "x11.h"
#include "serial.h"

Display *dsp;
Window win;
int screenNumber;
unsigned long white;
unsigned long black;
GC gc;
Colormap colormap;
XColor camColor;
Pixmap cameraMap;
Font font;

int xInit() {
	dsp = XOpenDisplay( NULL );
	if( !dsp )
		return 1;
	screenNumber = DefaultScreen(dsp);
	
	white = WhitePixel(dsp, screenNumber);
	black = BlackPixel(dsp, screenNumber);

	win = XCreateSimpleWindow(dsp, DefaultRootWindow(dsp), 0, 0, WIDTH, HEIGHT, 0, black, white);

	XMapWindow(dsp, win);
	XSelectInput(dsp, win, StructureNotifyMask);
	XEvent evt;
	do {
		XNextEvent(dsp, &evt);
	} while (evt.type != MapNotify);

	colormap = DefaultColormap(dsp, 0);
	gc = XCreateGC(dsp, win, 0, NULL);
	XSetForeground(dsp, gc, black);

	cameraMap = XCreatePixmap(dsp, win, 128, 128, DefaultDepth(dsp, screenNumber));

	XFlush(dsp);
	font = XLoadFont(dsp, "fixed");

	return 0;
}

void xClose() {
	XUnloadFont(dsp, font);
	XFreePixmap(dsp, cameraMap);
	XDestroyWindow(dsp, win);
	XCloseDisplay(dsp);
}
