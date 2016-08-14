#include <stdio.h>
#include <modbus.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <errno.h>
#include "font.h"
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	modbus_t *mb;
	uint16_t tab_reg[256];
	int rc;
	int i;

	if (argc == 1)
	{
		printf("name ip adress\n");
		exit(1);
	}

	mb = modbus_new_tcp(argv[1], 502);

	int rollpos = 0;
	int roll[3000];
	int roll2[3000];
	int roll3[3000];
	int roll4[3000];
	int roll5[3000];
	int roll6[3000];
	memset(roll,0,3000*sizeof(int));
	memset(roll2,0,3000*sizeof(int));
	memset(roll3,0,3000*sizeof(int));
	memset(roll4,0,3000*sizeof(int));
	memset(roll5,0,3000*sizeof(int));
	memset(roll6,0,3000*sizeof(int));
	int incomdata = 0;
	int incomdata2 = 0;
	int incomdata3 = 0;
	int incomdata4 = 0;
	int incomdata5 = 0;
	int incomdata6 = 0;
	char incomstring[100];
	char incomstring2[100];
	char incomstring3[100];
	char incomstring4[100];
	char incomstring5[100];
	char incomstring6[100];
	int xwidth = 300, yheight = 300;
	int x = 0;
	int y = 0;
	float xs = 1.0;
	float ys = 1.0;
	char buf[100];
	static GLfloat spin = 0.0;
	int dblBuf[] =  {GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_BLUE_SIZE, 1, GLX_DEPTH_SIZE, 12, GLX_DOUBLEBUFFER, None};
	XSetWindowAttributes swa;
	XEvent event;

	Display *dpy = XOpenDisplay(NULL);
	XVisualInfo *vi = glXChooseVisual(dpy, DefaultScreen(dpy), dblBuf);
	GLXContext cx = glXCreateContext(dpy, vi, None, True);
	Colormap cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen), vi->visual, AllocNone);
	swa.colormap = cmap;
	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask;
	Window win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, xwidth, yheight, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel | CWColormap | CWEventMask, &swa);
	XSetStandardProperties(dpy, win, "glxsimple", "glxsimple", None, argv, argc, NULL);
	glXMakeCurrent(dpy, win, cx);
	XSelectInput(dpy, win, KeyPressMask | StructureNotifyMask);
	XMapWindow(dpy, win);
	Atom WM_DELETE_WINDOW = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dpy, win, &WM_DELETE_WINDOW, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	GLuint fontOffset = glGenLists(128);
	for (GLuint i = 32; i < 127; i++)
	{
		glNewList(i+fontOffset, GL_COMPILE);
		glBitmap(8,13,0.0,2.0,10.0,0.0,font[i-32]);
		glEndList();
	}
	glListBase(fontOffset);
	glViewport(0, 0, xwidth, yheight);
	glLoadIdentity();
	gluOrtho2D(0.0,(GLdouble)xwidth,(GLdouble)yheight/-2,(GLdouble)yheight/2);
	while (1)
	{
		while (XPending(dpy))
		{
			XNextEvent(dpy, &event);
			switch (event.type)
			{
	      		case ConfigureNotify:
				xwidth = event.xconfigure.width;
				yheight = event.xconfigure.height;
				glViewport(0, 0, event.xconfigure.width, event.xconfigure.height);
				glLoadIdentity();
				gluOrtho2D(0.0,(GLdouble)event.xconfigure.width,(GLdouble)event.xconfigure.height/-2,(GLdouble)event.xconfigure.height/2);
				break;
			case ClientMessage:
				XCloseDisplay(dpy);
				exit(0);
				break;
			}
		}
	while (modbus_connect(mb) == -1)
	{
		fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
		//modbus_free(mb);
		//return -1;
	}
	rc = modbus_read_registers(mb, 0, 10, tab_reg);
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return -1;
	}
	modbus_close(mb);

	incomdata = tab_reg[0];
	incomdata2 = tab_reg[1];
	incomdata3 = tab_reg[2];
	incomdata4 = tab_reg[3];
	incomdata5 = tab_reg[4];
	incomdata6 = tab_reg[5];

		sprintf(incomstring, "%d",incomdata);
		sprintf(incomstring2, "%d",incomdata2);
		sprintf(incomstring3, "%d",incomdata3);
		sprintf(incomstring4, "%d",incomdata4);
		sprintf(incomstring5, "%d",incomdata5);
		sprintf(incomstring6, "%d",incomdata6);
		printf("%d %d %d %d %d %d\n",incomdata, incomdata2, incomdata3, incomdata4, incomdata5, incomdata6);
		roll[rollpos] = incomdata;
		roll2[rollpos] = incomdata2;
		roll3[rollpos] = incomdata3;
		roll4[rollpos] = incomdata4;
		roll5[rollpos] = incomdata5;
		roll6[rollpos] = incomdata6;
		rollpos++;
		if (rollpos == xwidth)
			rollpos = 0;

		glClear(GL_COLOR_BUFFER_BIT);
		//translate matrix
		glPushMatrix();
		glTranslatef(x,y,0.0);
		//scale matrix
		glPushMatrix();
		glScalef(xs,ys,1.0);
		//glRotatef(1.0,0.0,0.0,1.0);
		//rotate matrix
		glPushMatrix();
		//glLoadIdentity();
		//glRotatef(spin,0.0,0.0,1.0);
		glRasterPos2f(50.0,50.0);

		glCallLists(40, GL_UNSIGNED_BYTE,(GLubyte *)incomstring);
		glRasterPos2f(50.0,70.0);
		glCallLists(40, GL_UNSIGNED_BYTE,(GLubyte *)incomstring2);
		glRasterPos2f(50.0,90.0);
		glCallLists(40, GL_UNSIGNED_BYTE,(GLubyte *)incomstring3);
		glRasterPos2f(50.0,110.0);
		glCallLists(40, GL_UNSIGNED_BYTE,(GLubyte *)incomstring4);
		glRasterPos2f(50.0,130.0);
		glCallLists(40, GL_UNSIGNED_BYTE,(GLubyte *)incomstring5);
		glRasterPos2f(50.0,150.0);
		glCallLists(40, GL_UNSIGNED_BYTE,(GLubyte *)incomstring6);

		int i;
		glBegin(GL_LINE_STRIP);
			for (i=0; rollpos-i>0 ; i++)
				glVertex2f(i,roll[rollpos-i]);
			for (;i<xwidth; i++)
				glVertex2f(i,roll[rollpos-i+xwidth]);
		glEnd();
		glBegin(GL_LINE_STRIP);
			for (i=0; rollpos-i>0 ; i++)
				glVertex2f(i,roll2[rollpos-i]);
			for (;i<xwidth; i++)
				glVertex2f(i,roll2[rollpos-i+xwidth]);
		glEnd();
		glBegin(GL_LINE_STRIP);
			for (i=0; rollpos-i>0 ; i++)
				glVertex2f(i,roll3[rollpos-i]);
			for (;i<xwidth; i++)
				glVertex2f(i,roll3[rollpos-i+xwidth]);
		glEnd();
		glBegin(GL_LINE_STRIP);
			for (i=0; rollpos-i>0 ; i++)
				glVertex2f(i,roll4[rollpos-i]);
			for (;i<xwidth; i++)
				glVertex2f(i,roll4[rollpos-i+xwidth]);
		glEnd();
		glBegin(GL_LINE_STRIP);
			for (i=0; rollpos-i>0 ; i++)
				glVertex2f(i,roll5[rollpos-i]);
			for (;i<xwidth; i++)
				glVertex2f(i,roll5[rollpos-i+xwidth]);
		glEnd();
		glBegin(GL_LINE_STRIP);
			for (i=0; rollpos-i>0 ; i++)
				glVertex2f(i,roll6[rollpos-i]);
			for (;i<xwidth; i++)
				glVertex2f(i,roll6[rollpos-i+xwidth]);
		glEnd();
		glPopMatrix();
		glPopMatrix();
		glPopMatrix();

		glXSwapBuffers(dpy, win);
		glFlush();          
		usleep(200000);
		//sleep(1);
	}
	modbus_free(mb);
	return 0;
}
