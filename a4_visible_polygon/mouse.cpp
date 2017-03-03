/* mouse2.c

Laura Toma

What it does:  The user can enter a polygon by clicking on the mouse.

*/

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <vector>
#include "geom.h"
#include "vispoly.h"

using namespace std;

GLfloat red[3] = {1.0, 0.0, 0.0};
GLfloat green[3] = {0.0, 1.0, 0.0};
GLfloat blue[3] = {0.0, 0.0, 1.0};
GLfloat black[3] = {0.0, 0.0, 0.0};
GLfloat white[3] = {1.0, 1.0, 1.0};
GLfloat gray[3] = {0.5, 0.5, 0.5};
GLfloat yellow[3] = {1.0, 1.0, 0.0};
GLfloat magenta[3] = {1.0, 0.0, 1.0};
GLfloat cyan[3] = {0.0, 1.0, 1.0};

GLint fillmode = 0;

/* forward declarations of functions */
void display(void);
void keypress(unsigned char key, int x, int y);
void mousepress(int button, int state, int x, int y);
void timerfunc(int n);
void initialize_polygon();
void print_polygon(vector<point2D> poly);
void draw_polygon(vector<point2D> poly);
void reinitialize();

/* our coordinate system is (0,0) x (WINDOWSIZE,WINDOWSIZE) where the
   origin is the lower left corner */

/* global variables */
const int WINDOWSIZE = 750;

//the current polygon
vector<point2D> poly;
vector<point2D> vispoly;

//coordinates of last mouse click
double mouse_x=-10, mouse_y=-10;
//initialized to a point outside the window

//when this is 1, then clicking the mouse results in those points being stored in poly
bool poly_init_mode = false;

// toggles random walk for the timer function
bool walk = false;

void draw_circle(double x, double y){
  glColor3fv(blue);
  int precision = 100;
  double r = 4;
  double theta = 0;
  glBegin(GL_POLYGON);
  for(int i = 0; i < precision; i++){
    theta = i * 2 * M_PI/precision;
    glVertex2f(x + r*cos(theta), y + r*sin(theta));
  }
  glEnd();
}

/*
Usage

void glutMouseFunc(void (*func)(int button, int state, int x, int y));

Description

glutMouseFunc sets the mouse callback for the current window. When a
user presses and releases mouse buttons in the window, each press and
each release generates a mouse callback. The button parameter is one
of GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON. For
systems with only two mouse buttons, it may not be possible to
generate GLUT_MIDDLE_BUTTON callback. For systems with a single mouse
button, it may be possible to generate only a GLUT_LEFT_BUTTON
callback. The state parameter is either GLUT_UP or GLUT_DOWN
indicating whether the callback was due to a release or press
respectively. The x and y callback parameters indicate the window
relative coordinates when the mouse button state changed. If a
GLUT_DOWN callback for a specific button is triggered, the program can
assume a GLUT_UP callback for the same button will be generated
(assuming the window still has a mouse callback registered) when the
mouse button is released even if the mouse has moved outside the
window.
*/

void mousepress(int button, int state, int x, int y) {

  if(state == GLUT_DOWN) {
    walk = false; // stop the random walk
    mouse_x = x;
    mouse_y = y;
    //(x,y) are in wndow coordinates, where the origin is in the upper
    //left corner; our reference system has the origin in lower left
    //corner, this means we have to reflect y
    mouse_y = WINDOWSIZE - mouse_y;

    printf("mouse click at (x=%d, y=%d)\n", (int)mouse_x, (int)mouse_y);

    point2D p = {mouse_x, mouse_y};

    if (poly_init_mode == true) {
      poly.push_back(p);
    }
    else { //poly_init_mode == false
      vispoly = find_vispoly(p, poly);
    }
  }

  glutPostRedisplay();
}

/* ****************************** */
/* initialize  polygon stored in global variable poly  */
void initialize_polygon() {

  //clear the vector, in case something was there
  poly.clear();
  vispoly.clear();

  int n = 10; //size of polygon
  double rad = 100;
  double step = 2 * M_PI / n;
  point2D p;
  for (int i=0; i<n; i++) {

    p.x = WINDOWSIZE/2 + rad * cos (i * step);
    p.y = WINDOWSIZE/2 + rad * sin (i * step);

    //insert the segment in the array of segments
    poly.push_back (p);
  } //for i
}

/* ************************************************** */
void print_polygon(vector<point2D> poly) {

  for (unsigned int i=0; i<poly.size()-1; i++) {
    printf("edge %d: [(%d,%d), (%d,%d)]\n",
	   i, poly[i].x, poly[i].y, poly[i+1].x, poly[i+1].y);
  }
  //print last edge from last point to first point
  int last = poly.size()-1;
    printf("edge %d: [(%d,%d), (%d,%d)]\n",
	   last, poly[last].x, poly[last].y, poly[0].x, poly[0].y);

}

/* ****************************** */
int main(int argc, char** argv) {

  srand(time(NULL));
  initialize_polygon();
  print_polygon(poly);

  /* initialize GLUT  */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
  glutInitWindowPosition(100,100);
  glutCreateWindow(argv[0]);

  /* register callback functions */
  glutDisplayFunc(display);
  glutKeyboardFunc(keypress);
  glutMouseFunc(mousepress);

  /* init GL */
  /* set background color black*/
  glClearColor(0, 0, 0, 0);
  /* here we can enable depth testing and double buffering and so
     on */


  /* give control to event handler */
  glutMainLoop();
  return 0;
}

/* ****************************** */
/* draw the polygon */
void draw_polygon(vector<point2D> poly){
  if (poly.size() == 0) return;

  int i;
  for (i=0; i<poly.size()-1; i++) {
    glBegin(GL_LINES);
    glVertex2f(poly[i].x, poly[i].y);
    glVertex2f(poly[i+1].x, poly[i+1].y);
    glEnd();
  }
  //render last segment between last point and forst point
  int last=poly.size()-1;
  glBegin(GL_LINES);
  glVertex2f(poly[last].x, poly[last].y);
  glVertex2f(poly[0].x, poly[0].y);
  glEnd();
}

/* draws the visible polygon: has special property that every vertex
   is guarenteed visible from the guard */
void draw_visible_polygon(int guard_x, int guard_y, vector<point2D> vispoly){
  if (vispoly.size() == 0) return;

  glColor3fv(red);
  int i;
  for (i=0; i<vispoly.size()-1; i++) {
    glBegin(GL_TRIANGLES);
    point2D a = vispoly[i];
    point2D b = vispoly[i+1];

    glVertex2f(guard_x, guard_y);
    glVertex2f(a.x, a.y);
    glVertex2f(b.x, b.y);
    glEnd();
  }

  //render the last triangle between the last and first vertex
  point2D a = vispoly[0];
  point2D b = vispoly[i];

  glBegin(GL_TRIANGLES);
  glVertex2f(guard_x, guard_y);
  glVertex2f(a.x, a.y);
  glVertex2f(b.x, b.y);
  glEnd();
}

/* ****************************** */
void display(void) {

  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); //clear the matrix

  /* The default GL window is [-1,1]x[-1,1] with the origin in the
     center.

     Our system of coordinates (in which we generate our points) is
     (0,0) to (WINSIZE,WINSIZE), with the origin in the lower left
     corner.

     We need to map the points to [-1,1] x [-1,1]

     Assume we are the local coordinate system.

     First we scale down to [0,2] x [0,2] */
  glScalef(2.0/WINDOWSIZE, 2.0/WINDOWSIZE, 1.0);
   /* Then we translate so the local origin goes in the middle of teh
     window to (-WINDOWSIZE/2, -WINDOWSIZE/2) */
  glTranslatef(-WINDOWSIZE/2, -WINDOWSIZE/2, 0);

  //now we draw in our local coordinate system (0,0) to
  //(WINSIZE,WINSIZE), with the origin in the lower left corner.

  glColor3fv(yellow);
  draw_polygon(poly);
  draw_visible_polygon(mouse_x, mouse_y, vispoly);

  //draw a circle where the mouse was last clicked. Note that this
  //point is stored as a global variable and is modified by the mouse
  //handler function
  draw_circle(mouse_x, mouse_y);

  /* execute the drawing commands */
  glFlush();
}

/* ****************************** */
//expected behaviour: press 's', then click on the points you
//want, and press 'e' when you're done. the points will be saved
//in global 'poly'. press 'w' to start random walk
void keypress(unsigned char key, int x, int y) {
  switch(key) {
  case 'q':
    exit(0);
    break;

  case 's':
    poly_init_mode = true;
    walk = false;
    reinitialize();
    break;

  case 'e':
    poly_init_mode = false;
    glutPostRedisplay();
    break;

  case 'w':
    if (walk == true) {
      walk = false;
    }
    else {
      walk = true;
      timerfunc(10);
    }
    printf("make the guard walk here");

  }
}

// clears the screen
void reinitialize() {
  poly.clear();
  vispoly.clear();
  mouse_x = mouse_y=0;
  glutPostRedisplay();
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer

   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);

   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset
   gluOrtho2D(0.0, (GLdouble) width, 0.0, (GLdouble) height);
}

void timerfunc(int n) {
  printf("asdfasdf asdf asdf \n");
  int dx = rand() % 3 - 1;
  int dy = rand() % 3 - 1;

  if (dx != 0 || dy != 0) {
    int temp_mouse_x = mouse_x+(dx*5);
    int temp_mouse_y = mouse_y+(dy*5);
    point2D p = {temp_mouse_x, temp_mouse_y};

    printf("mousex is %d and mousey is %d \n", temp_mouse_x, temp_mouse_y);

    if (guard_in_poly(p, poly)) {
      mouse_x = temp_mouse_x;
      mouse_y = temp_mouse_y;

      //reset the vispolygon
      vispoly.clear();
      glutPostRedisplay();

      vispoly = find_vispoly(p, poly);
      draw_circle(p.x, p.y);
      draw_visible_polygon(mouse_x, mouse_y, vispoly);
      glutPostRedisplay();

    }
  }

  if(walk == true) glutTimerFunc(n, timerfunc, n);
}
