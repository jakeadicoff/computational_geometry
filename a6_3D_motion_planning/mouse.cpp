/* mouse2.c

Laura Toma

What it does:  The user can enter a polygon by clicking on the mouse.

*/

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <vector>
#include "geom.h"
#include "motionPlan.h"

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

void initialize_robot(point2D start, vector<point2D> *this_robot);
void print_polygon(vector<point2D> poly);
void draw_polygons(vector<vector<point2D> > poly_set);
void reset_robots();
void reinitialize_poly();
void reinitialize_poly_set();
void initialize_polygons();

/* our coordinate system is (0,0) x (WINDOWSIZE,WINDOWSIZE) where the
   origin is the lower left corner */

/* global variables */
const int WINDOWSIZE = 750;

//the current polygon
vector<vector < point2D > > poly_set;
vector<point2D> poly;

vector<point2D> start_robot;
vector<point2D> end_robot;

bool animate = false;
int pathstep;
vector<Position> path;
vector<point2D> moving_bot;

//coordinates of last mouse click
double mouse_x=-10, mouse_y=-10;
//initialized to a point outside the window

//when this is 1, then clicking the mouse results in those points being stored in poly
bool poly_init_mode = false;
bool points_init_mode = true;
bool init_start = true;

// global points to store start and end points of path
point2D start_point = {-10, -10};
point2D end_point = {-10, -10};

void mousepress(int button, int state, int x, int y) {
  if(state == GLUT_DOWN) {
    mouse_x = x;
    mouse_y = y;
    //(x,y) are in wndow coordinates, where the origin is in the upper
    //left corner; our reference system has the origin in lower left
    //corner, this means we have to reflect y
    mouse_y = WINDOWSIZE - mouse_y;

    //    printf("mouse click at (x=%d, y=%d)\n", (int)mouse_x, (int)mouse_y);

    point2D p = {mouse_x, mouse_y};

    if (poly_init_mode == true) {
      poly.push_back(p);
      if(poly.size() > 1) {
	poly_set.pop_back();
      }
      poly_set.push_back(poly);
    }
    else if(points_init_mode == true) {
      if(init_start) {
	reset_robots();

	init_start = false;
	p.x = p.x -(int) p.x%3;
	p.y = p.y -(int) p.y%3;
	start_point = p;
	initialize_robot(start_point, &start_robot);

	printf("Please click an 'end' point \n");
      }
      else {
	init_start = true;
	end_point = p;
	initialize_robot(end_point, &end_robot);

	path = find_shortest_path(start_robot, end_robot, poly_set);

	

	printf("\nHit 'a' to begin drawing polygons \n"
	       "Hit 'f' at any time to bring up the default scene \n"
	       "Please click a 'start' point \n");
      }
    } // elif points init mode == true
  }

  glutPostRedisplay();
}

void initialize_robot(point2D start, vector<point2D> *this_robot) {
  vector<point2D> temp_robot;
  temp_robot.push_back(start);

  start.x += 10;
  temp_robot.push_back(start);

  start.y += 40;
  temp_robot.push_back(start);

  start.x -= 10;
  temp_robot.push_back(start);

  *this_robot = temp_robot;
}

/* ****************************** */
/* draw the polygon */
void draw_polygons(vector <vector <point2D> > poly_set){
  if (poly_set.size() == 0) return;
  for(int h = 0; h < poly_set.size(); h++) {
    vector<point2D> polyCopy = poly_set[h]; //do we need this
    int i;
    for (i=0; i<polyCopy.size()-1; i++) {
      glBegin(GL_LINES);
      glVertex2f(polyCopy[i].x, polyCopy[i].y);
      glVertex2f(polyCopy[i+1].x, polyCopy[i+1].y);
      glEnd();
    }
    //render last segment between last point and forst point
    int last=polyCopy.size()-1;
    glBegin(GL_LINES);
    glVertex2f(polyCopy[last].x, polyCopy[last].y);
    glVertex2f(polyCopy[0].x, polyCopy[0].y);
    glEnd();
  }
}

/* ************************************************** */
void print_polygon(vector<point2D> poly) {
  for (unsigned int i=0; i<poly.size()-1; i++) {
    printf("edge %d: [(%f,%f), (%f,%f)]\n",
	   i, poly[i].x, poly[i].y, poly[i+1].x, poly[i+1].y);
  }
  //print last edge from last point to first point
  int last = poly.size()-1;
    printf("edge %d: [(%f,%f), (%f,%f)]\n",
	   last, poly[last].x, poly[last].y, poly[0].x, poly[0].y);
}

void draw_circle(double x, double y){
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

/* ****************************** */
int main(int argc, char** argv) {
  srand(time(NULL));
  initialize_polygons();
  printf("\nHit 'a' to begin drawing polygons \n"
	 "Hit 'f' at any time to bring up the default scene \n"
	 "Please click a 'start' point \n");
  //  print_polygon(poly);

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

  //draw a circle where the mouse was last clicked. Note that this
  //point is stored as a global variable and is modified by the mouse
  //handler function
  glColor3fv(blue);
  draw_circle(mouse_x, mouse_y);

  glColor3fv(yellow);
  glLineWidth(3);
  draw_polygons(poly_set);

  // draw robots at start and end points
  glLineWidth(1);
  glColor3fv(cyan);

  vector<vector<point2D> > robots;
  if(start_robot.size() == 4)
    robots.push_back(start_robot);
  if(end_robot.size() == 4)
    robots.push_back(end_robot);
  draw_polygons(robots);

  /* execute the drawing commands */
  glFlush();
}

/* ****************************** */
void keypress(unsigned char key, int x, int y) {
  switch(key) {
  case 'q':
    exit(0);
    break;

  case 'a':
    reset_robots();
    reinitialize_poly_set();
    poly_init_mode = true;

    printf("\nClick to draw a polygon, "
	   "and hit 's' to begin drawing the next one. \n"
	   "Hit 'd' to stop drawing polygons. \n");
    break;

  case 's':
    reinitialize_poly();
    break;

  case 'd':
    reset_robots();
    poly_init_mode = false;
    glutPostRedisplay();

    printf("\nPlease click a 'start' point\n");
    break;

  case 'f':
    reset_robots();
    init_start = true;
    poly_init_mode = false;

    initialize_polygons();
    printf("\nPlease click a 'start' point \n");
    glutPostRedisplay();
    break;

  case 'w':
    if(path.size() != 0) {
      animate = true;
      pathstep = path.size()-1;
      timerfunc(3);
      break;
    }
  }
}

// reset the start and end points
void reset_robots() {
  start_robot.clear();
  end_robot.clear();
  path.clear();
}

// clears the screen
void reinitialize_poly() {
  poly.clear();
  mouse_x = mouse_y=0;
  glutPostRedisplay();
}

void reinitialize_poly_set() {
  poly.clear();
  poly_set.clear();
  mouse_x = mouse_y=0;
  start_point.x = start_point.y = end_point.x = end_point.y = -10;
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
  glutPostRedisplay();

  Position curr_pos = path[pathstep];
  moving_bot = place_bot(curr_pos.x*3, curr_pos.y*3, curr_pos.t*4, start_robot);

  //draw the moving robot
  glLineWidth(1);
  glColor3fv(red);

  vector<vector<point2D> > moving_bot_vec;
  if(moving_bot.size() == 4) {
    moving_bot_vec.push_back(moving_bot);
  }

  draw_polygons(moving_bot_vec);

  glFlush();

  if(pathstep == 0 || path.size() == 0) animate = false;
  pathstep--;
  if(animate == true) glutTimerFunc(n, timerfunc, n);
}

/* ****************************** */
/* initialize polygon set with a cool looking set of polygons */
void initialize_polygons() {
  //clear the vector, in case something was there
  poly.clear();
  poly_set.clear();

  point2D p;

  p.x = 50;
  p.y = 50;
  poly.push_back(p);
  p.x = 50;
  p.y = 700;
  poly.push_back(p);
  p.x = 100;
  p.y = 700;
  poly.push_back(p);
  p.x = 100;
  p.y = 150;
  poly.push_back(p);
  p.x = 500;
  p.y = 150;
  poly.push_back(p);
  p.x = 500;
  p.y = 400;
  poly.push_back(p);
  p.x = 550;
  p.y = 400;
  poly.push_back(p);
  p.x = 550;
  p.y = 50;
  poly.push_back(p);

  poly_set.push_back(poly);
  poly.clear();

  p.x = 400;
  p.y = 300;
  poly.push_back(p);
  p.x = 300;
  p.y = 400;
  poly.push_back(p);
  p.x = 250;
  p.y = 250;
  poly.push_back(p);
  p.x = 175;
  p.y = 175;
  poly.push_back(p);

  poly_set.push_back(poly);
  poly.clear();

  p.x = 699;
  p.y = 699;
  poly.push_back(p);
  p.x = 700;
  p.y = 600;
  poly.push_back(p);
  p.x = 475;
  p.y = 600;
  poly.push_back(p);
  p.x = 475;
  p.y = 200;
  poly.push_back(p);
  p.x = 375;
  p.y = 200;
  poly.push_back(p);
  p.x = 450;
  p.y = 450;
  poly.push_back(p);
  p.x = 450;
  p.y = 650;
  poly.push_back(p);

  poly_set.push_back(poly);
  poly.clear();

  p.x = 250;
  p.y = 600;
  poly.push_back(p);
  p.x = 350;
  p.y = 600;
  poly.push_back(p);
  p.x = 300;
  p.y = 650;
  poly.push_back(p);

  poly_set.push_back(poly);
  poly.clear();

  p.x = 150;
  p.y = 400;
  poly.push_back(p);
  p.x = 250;
  p.y = 500;
  poly.push_back(p);
  p.x = 400;
  p.y = 550;
  poly.push_back(p);

  poly_set.push_back(poly);
  poly.clear();

  p.x = 700;
  p.y = 50;
  poly.push_back(p);
  p.x = 590;
  p.y = 100;
  poly.push_back(p);
  p.x = 625;
  p.y = 200;
  poly.push_back(p);

  poly_set.push_back(poly);
  poly.clear();

  p.x = 700;
  p.y = 150;
  poly.push_back(p);
  p.x = 590;
  p.y = 400;
  poly.push_back(p);
  p.x = 625;
  p.y = 575;
  poly.push_back(p);

  poly_set.push_back(poly);
  poly.clear();
}
