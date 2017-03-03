/* view.c

Laura Toma

What it does:

Draws a set of horizontal and vertical line segments in the default 2D
projection. Then computes their intersections using the line sweep
algorithm, and  simulates the algorithm as it runs.

*/

#include "geom.h"
#include "rtimer.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <vector>
#include <algorithm>
#include <unistd.h>

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
void timerfunc();
void initialize_segments();
void draw_sweep_line();
void initialize_segments_random(); // Toma's test cases
void initialize_segments_horizontal();
void initialize_segments_square(); // My test case
void initialize_segments_vertical(); // Andrew's test cases
void initialize_segments_grid();
void initialize_segments_overlap();
void initialize_segments_overlap2(); // Jeonguk's test case
void initialize_segments_fourintersections(); // James's test case
void print_segments();
void run_search();

//renders the intersection points
void draw_intersection_points();

/* global variables */
const int WINDOWSIZE = 500;

int init_case = 0;
const int NB_TEST_CASES = 7;

//NOTE: all the structures below need to be global so that they can be rendered

//number of segments requested by user
int n;

//current position of sweep line
int sweep_line_x = 0;

//the array of  segments
vector<segment2D>  segments;

//the intersections points of the segments
vector<point2D> intpoints;
vector<point2D> intpoints_copy;

/* ************************************************** */
//fills global variable "segments" with n segments
void initialize_segments() {
  switch (init_case)  {
  case 0:
    initialize_segments_random();
    break;
  case 1:
    initialize_segments_horizontal();
    break;
  case 2:
    initialize_segments_square();
    break;
  case 3:
    initialize_segments_vertical();
    break;
  case 4:
    initialize_segments_grid();
    break;
  case 5:
     initialize_segments_overlap();
    break;
  case 6:
    initialize_segments_overlap2();
  case 7:
    initialize_segments_fourintersections();
  default:
    initialize_segments_random();
  }

  init_case = (init_case+1) % NB_TEST_CASES;
  return;
}

/* ************************************************** */
//fills global variable "segments" with n horizontal segments
void initialize_segments_horizontal() {
  int i;
  point2D a,b;
  segment2D s;

  //clear the vector
  segments.clear();

  //a long horizontal segment
  a.x = 1;
  a.y = WINDOWSIZE/2;
  b.x = WINDOWSIZE - 10;
  b.y = a.y;

  s.start = a; s.end = b;
  segments.push_back(s);

  //n-1 vertical segments
  for (i=0; i<n-1; i++) {

    a.x = i*WINDOWSIZE/n;
    a.y = WINDOWSIZE/2 - random() % ((int)(.4*WINDOWSIZE));
    b.x = a.x;
    b.y = WINDOWSIZE/2 + random() % ((int)(.4*WINDOWSIZE));
    s.start = a; s.end = b;
    segments.push_back(s);
  }

}

/* ****************************** */
//fills global variable "segments" with n random segments
void initialize_segments_random() {
  //clear the vector
  segments.clear();

  int i;
  point2D a, b;
  segment2D s;
  for (i=0; i<n; i++) {
    if (random()%2 == 0) {
      //horizontal segment
      a.x = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
      a.y =  (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
      b.y = a.y;
      b.x = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));

      if (a.x < b.x) {
	s.start = a; s.end = b;
      } else {
	s.start = b; s.end = a;
      }

    } else {
      //vertical segment
      a.x = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
      b.x = a.x;
      a.y = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));
      b.y = (int)(.3*WINDOWSIZE)/2 + random() % ((int)(.7*WINDOWSIZE));

      if (a.y < b.y) {
	s.start = a; s.end = b;
      } else {
	s.start = b; s.end = a;
      }
    }

    //insert the segment in the array of segments
    segments.push_back (s);
  } //for i
}

//fills global variable "segments" with a square
void initialize_segments_square() {
  //clear the vector
  segments.clear();

  int xi, xf, yi, yf;
  point2D a,b;
  segment2D s;

  xi = 100;
  xf = 200;
  yi = 100;
  yf = 200;

  for(int i=0; i < 4; i++) {
    switch(i) {
    case 0: //top seg
      a.x = xi;
      a.y = yf;
      b.x = xf;
      b.y = yf;
      break;
    case 1: //bottom seg
      a.x = xi;
      a.y = yi;
      b.x = xf;
      b.y = yi;
      break;
    case 2: //left seg
      a.x = xi;
      a.y = yf;
      b.x = xi;
      b.y = yi;
      break;
    case 3: //right seg
      a.x = xf;
      a.y = yf;
      b.x = xf;
      b.y = yi;
      break;
    }
    s.start = a; s.end = b;
    segments.push_back(s);
  }
}

/* Next three tests are Andrew's test cases */

//fills global variable "segments" with 1 vertical segment and n-1
//horizontal segments
void initialize_segments_vertical() {
  segments.clear();

  int i;
  point2D a,b;
  segment2D s;

  //a long vertical segment
  a.y = 1;
  a.x = WINDOWSIZE/2;
  b.y = WINDOWSIZE - 10;
  b.x = a.x;

  s.start = a; s.end = b;
  segments.push_back(s);

  //n-1 horizontal segments
  for (i=0; i<n-1; i++) {

    a.y = i*WINDOWSIZE/n;
    a.x = WINDOWSIZE/2 - random() % ((int)(.4*WINDOWSIZE));
    b.y = a.y;
    b.x = WINDOWSIZE/2 + random() % ((int)(.4*WINDOWSIZE));

    if (a.x < b.x) {
      s.start = a; s.end = b;
    } else {
      s.start = b; s.end = a;
    }

    segments.push_back(s);
  }

}

//fills the global variable "segments" with an n x n grid
void initialize_segments_grid() {
  int i;
  point2D a,b;
  segment2D s;

  //clear the vector
  segments.clear();

  //n-1 vertical segments
  for (i=0; i < n; i++) {
    if (i % 2 == 0) {
      a.x = (2 * i) % (WINDOWSIZE / 2) + (WINDOWSIZE / 4);
      a.y = (WINDOWSIZE / 4);
      b.x = a.x;
      b.y = (WINDOWSIZE / 4) + (WINDOWSIZE / 2);
    }
    else {
      a.y = (2 * i) % (WINDOWSIZE / 2) + WINDOWSIZE / 4;
      a.x = (WINDOWSIZE / 4);
      b.y = a.y;
      b.x = (WINDOWSIZE / 4) + (WINDOWSIZE / 2);
    }
    s.start = a; s.end = b;
    segments.push_back(s);
  }
}

//fills the global variable "segments" with a 3x3 grid of segments that meet
//at their endpoints
void initialize_segments_overlap() {
  int i;
  point2D a,b;
  segment2D s;

  segments.clear();

  //n-1 vertical segments
  for (i=0; i < n; i++) {
    if (i % 6 == 0) {
      a.x = WINDOWSIZE/4;
      a.y = WINDOWSIZE/4;
      b.x = a.x;
      b.y = 3*WINDOWSIZE/4;
    }
    else if (i % 6 == 1) {
      a.x = WINDOWSIZE/2;
      a.y = WINDOWSIZE/4;
      b.x = a.x;
      b.y = 3*WINDOWSIZE/4;
    }
    else if (i % 6 == 2) {
      a.x = 3*WINDOWSIZE/4;
      a.y = WINDOWSIZE/4;
      b.x = a.x;
      b.y = 3*WINDOWSIZE/4;
    }
    else if (i % 6 == 3) {
      a.y = WINDOWSIZE/4;
      a.x = WINDOWSIZE/4;
      b.y = a.y;
      b.x = 3*WINDOWSIZE/4;
    }
    else if (i % 6 == 4) {
      a.y = WINDOWSIZE/2;
      a.x = WINDOWSIZE/4;
      b.y = a.y;
      b.x = 3*WINDOWSIZE/4;
    }
    else {
      a.y = 3*WINDOWSIZE/4;
      a.x = WINDOWSIZE/4;
      b.y = a.y;
      b.x = 3*WINDOWSIZE/4;
    }

    s.start = a; s.end = b;
    segments.push_back(s);
  }
}

/* Jeonguk's test case */
void initialize_segments_overlap2() {
  segments.clear();
  int i;
  point2D a,b;
  segment2D s;

  for (i = 0; i < 10; i++) {
    a.x = 1 + i * (WINDOWSIZE/20);
    b.x = WINDOWSIZE - 1 - i*(WINDOWSIZE/20);
    a.y = WINDOWSIZE/2;
    b.y = a.y;

    s.start = a;
    s.end = b;
    segments.push_back(s);
  }

  for (i = 1; i < 4; i++) {
    a.x = 1 + i * WINDOWSIZE / 4;
    b.x = a.x;
    a.y = 1;
    b.y = WINDOWSIZE - 1;

    s.start = a;
    s.end = b;
    segments.push_back(s);
  }
}

/* James's test case */
void initialize_segments_fourintersections(){
  segments.clear();
  point2D a,b,c,d;
  a.x=200;
  a.y=150;
  b.x=250;
  b.y=150;

  segment2D temp;
  temp.start=a;
  temp.end=b;
  segments.push_back(temp);
  segments.push_back(temp);
  c.x=225;
  c.y=175;
  d.x=225;
  d.y=150;
  temp.start=d;
  temp.end=c;
  segments.push_back(temp);
  segments.push_back(temp);
}

/* ************************************************** */
void print_segments() {
  for (int i=0; i<segments.size(); i++) {
    printf("segment %d: [(%d,%d), (%d,%d)]\n",
	   i, segments[i].start.x, segments[i].start.y, segments[i].end.x, segments[i].end.y);
  }
}

/* ****************************** */
int main(int argc, char** argv) {
  //read number of points from user
  if (argc!=2) {
    printf("usage: viewPoints <nbPoints>\n");
    exit(1);
  }
  n = atoi(argv[1]);
  printf("you entered n=%d\n", n);
  assert(n >0);

  initialize_segments();
  //  print_segments();

  //runs the intersection search found in geom.h
  run_search();

  /* initialize GLUT  */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
  glutInitWindowPosition(100,100);
  glutCreateWindow(argv[0]);

  /* register callback functions */
  glutDisplayFunc(display);
  glutKeyboardFunc(keypress);
  glutIdleFunc(timerfunc);

  /* init GL */
  /* set background color black*/
  glClearColor(0, 0, 0, 0);
  /* here we can enable depth testing and double buffering and so
     on */

  /* give control to event handler */
  glutMainLoop();
  return 0;
}

/* runs orthointersect_search and times it*/
void run_search() {
  intpoints_copy.clear();

  Rtimer rt1;
  rt_start(rt1);
  // gets line intersections
  intpoints = orthointersect_search(segments);
  reverse(intpoints.begin(), intpoints.end());
  rt_stop(rt1);

  //print the timing
  char buf [1024];
  rt_sprint(buf,rt1);
  printf("run time:  %s\n\n", buf);
  fflush(stdout);
}

/* ****************************** */
/* draw the segments stored in global variable segments */
void draw_segments(){
  //set color
  glColor3fv(yellow);

  int i;
  for (i=0; i<segments.size(); i++) {
    glBegin(GL_LINES);
    glVertex2f(segments[i].start.x, segments[i].start.y);
    glVertex2f(segments[i].end.x, segments[i].end.y);
    glEnd();
  }
}

//draw a segment with current color
void draw_segment(segment2D s) {
  glBegin(GL_LINES);
  glVertex2f(s.start.x, s.start.y);
  glVertex2f(s.end.x, s.end.y);
  glEnd();
}

//draw the sweep line
void draw_sweep_line() {
  //sweep line color
  glColor3fv(cyan);

  //the current position of sweep line is sweep_line_x; assume it's a
  //segment from y=0 to y=windowsize;
  glBegin(GL_LINES);
  glVertex2f(sweep_line_x, 0);
  glVertex2f(sweep_line_x, WINDOWSIZE);
  glEnd();
}

//draw all the elements in the active structure
void draw_active_structure() {
  //TODO: Walk through the Set object and draw over every line??
}

//draw all the elements in intpoints
void draw_intersection_points() {
  const int R = 2;
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  //set color
  glColor3fv(red);

  int i;

  //only render points in sweep line order for small inputs
  if(n<10000) {
    for (i=intpoints.size()-1; i>=0; i--) {
      if(intpoints[i].x > sweep_line_x) {
	break;
      }
      intpoints_copy.push_back(intpoints[i]);
      intpoints.pop_back();
    }

    for (i=0; i<intpoints_copy.size(); i++) {
      //draw a small square centered at (points[i].x, points[i].y)
      glBegin(GL_POLYGON);
      glVertex2f(intpoints_copy[i].x -R,intpoints_copy[i].y-R);
      glVertex2f(intpoints_copy[i].x +R,intpoints_copy[i].y-R);
      glVertex2f(intpoints_copy[i].x +R,intpoints_copy[i].y+R);
      glVertex2f(intpoints_copy[i].x -R,intpoints_copy[i].y+R);
      glEnd();
    }
  }
  else {
    for (i=0; i<intpoints.size(); i++) {
      //draw a small square centered at (points[i].x, points[i].y)
      glBegin(GL_POLYGON);
      glVertex2f(intpoints[i].x -R,intpoints[i].y-R);
      glVertex2f(intpoints[i].x +R,intpoints[i].y-R);
      glVertex2f(intpoints[i].x +R,intpoints[i].y+R);
      glVertex2f(intpoints[i].x -R,intpoints[i].y+R);
      glEnd();
    }
  }
}

/* ****************************** */
void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity(); //clear the matrix

  /* The default GL window is [-1,1]x[-1,1] with the origin in the
     center.

     The points are in the range (0,0) to (WINSIZE,WINSIZE), so they
     need to be mapped to [-1,1]x [-1,1]x */
  glScalef(2.0/WINDOWSIZE, 2.0/WINDOWSIZE, 1.0);
  glTranslatef(-WINDOWSIZE/2, -WINDOWSIZE/2, 0);

  draw_segments();
  draw_active_structure();
  draw_intersection_points();
  //only render sweep line for small inputs: too slow to repeatedly
  //redraw thousands of points
  if(n<10000) draw_sweep_line();


  /* execute the drawing commands */
  glFlush();
}

/* ****************************** */
void keypress(unsigned char key, int x, int y) {
  switch(key) {
  case 'q':
    exit(0);
    break;

  case 'i':
    sweep_line_x = 0;
    initialize_segments();
    run_search();
    break;
  }
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

void timerfunc() {
  if(sweep_line_x < 500) sweep_line_x++;
  if(n<10000) usleep(5000);
  glutPostRedisplay();
}
