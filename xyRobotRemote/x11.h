#define WIDTH 300
#define HEIGHT 250

#include <X11/Xlib.h>

extern Display *dsp;
extern Window win;
extern int screenNumber;
extern unsigned long white;
extern unsigned long black;
extern GC gc;
extern Colormap colormap;
extern XColor camColor;
extern Pixmap cameraMap;
extern Font font;

int xInit();
void xClose();
