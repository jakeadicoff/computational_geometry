/*

 What it does:

 - generates a set of random points in 2D, builds a kd-tree on it and
 renders it in 2D with default orthogonal projection.

 - program is run as:  ./viewpoints <nbpoints>

 - when the user presses the space bar, it regenarates the point set of same size

 */

#include "rtimer.h"
#include "kdtree.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

GLfloat red[3] = {1.0, 0.0, 0.0};
GLfloat green[3] = {0.0, 1.0, 0.0};
GLfloat blue[3] = {0.0, 0.0, 1.0};
GLfloat black[3] = {0.0, 0.0, 0.0};
GLfloat white[3] = {1.0, 1.0, 1.0};
GLfloat gray[3] = {0.5, 0.5, 0.5};
GLfloat yellow[3] = {1.0, 1.0, 0.0};
GLfloat magenta[3] = {1.0, 0.0, 1.0};
GLfloat cyan[3] = {0.0, 1.0, 1.0};

/* forward declarations of functions */
void display(void);
void draw_rectangle(int top, int bottom, int left, int right);
void keypress(unsigned char key, int x, int y);
void reset();

/* global variables */
const int WINDOWSIZE = 500;
const int POINT_SIZE  = 6.0f;

//the array of n points
point2D*  points = NULL;
int n;
int iteration = 0;

// the kd-tree created with the points
kdtree *tree = NULL;

/* ****************************** */
/* initialize  the array of points stored in global variable points[] with random points */
void initialize_points_random() {
  //allocate points
  points = malloc(n * sizeof(point2D));
  assert(points);

  int i;
  for (i=0; i<n; i++) {
    points[i].x = (int)(.1*WINDOWSIZE)/2 + rand() % ((int)(.9*WINDOWSIZE));
    points[i].y =  (int)(.1*WINDOWSIZE)/2 + rand() % ((int)(.9*WINDOWSIZE));
  }
}

//Andrew's test cases
/* ****************************** */
/* initialize the array of points stored in global variable points[] with vertical points */
void initialize_points_vertical() {
  //allocate points
  points = malloc(n * sizeof(point2D));
  assert(points);

  int i;
  for (i=0; i<n; i++) {
    points[i].x = WINDOWSIZE / 2;
    points[i].y = rand() % (WINDOWSIZE / 2) + (WINDOWSIZE / 4);
  }
}

/* ****************************** */
/* initialize the array of points stored in global variable points[] with horizontal points */
void initialize_points_horizontal() {
  //allocate points
  points = malloc(n * sizeof(point2D));
  assert(points);

  int i;
  for (i=0; i<n; i++) {
    points[i].y = WINDOWSIZE / 2;
    points[i].x = rand() % (WINDOWSIZE / 2) + (WINDOWSIZE / 4);
  }
}

/* ****************************** */
/* initialize the array of points stored in global variable points[] with points at a right angle */
void initialize_points_right_angle() {
  //allocate points
  points = malloc(n * sizeof(point2D));
  assert(points);

  points[0].x = 2 * WINDOWSIZE / 3;
  points[0].y = 2 * WINDOWSIZE / 3;

  int i;
  for (i=1; i < n / 2; i++) {
    points[i].y = 2 * WINDOWSIZE / 3;
    points[i].x = rand() % (WINDOWSIZE / 3) + (WINDOWSIZE / 3);
  }
  for (i = n / 2; i < n; i++) {
    points[i].x = 2 * WINDOWSIZE / 3;
    points[i].y = rand() % (WINDOWSIZE / 3) + (WINDOWSIZE / 3);
  }
}

/* ****************************** */
/* initialize the array of points stored in global variable points[] with points in a cross */
void initialize_points_cross() {
  //allocate points
  points = malloc(n * sizeof(point2D));
  assert(points);

  int i;
  for (i=0; i < n / 2; i++) {
    points[i].y = WINDOWSIZE / 2;
    points[i].x = rand() % (WINDOWSIZE / 2) + (WINDOWSIZE / 4);
  }
  for (i = n / 2; i < n; i++) {
    points[i].x = WINDOWSIZE / 2;
    points[i].y = rand() % (WINDOWSIZE / 2) + (WINDOWSIZE / 4);
  }
}

void initialize_points() {
  switch(iteration) {
  case 0:
    initialize_points_random();
    break;
  case 1:
    initialize_points_vertical();
    break;
  case 2:
    initialize_points_horizontal();
    break;
  case 3:
    initialize_points_right_angle();
    break;
  case 4:
    initialize_points_cross();
    break;
  }
  iteration = (iteration + 1)%5;
}

point2D* points_cpy (point2D* points, int n) {
  point2D *pointcpy = NULL;
  pointcpy = malloc(n * sizeof(point2D));

  int i;
  for (i=0; i<n; i++) {
    pointcpy[i].x = points[i].x;
    pointcpy[i].y = points[i].y;
  }

  return pointcpy;
}


/* ****************************** */
/* print the array of points stored in global variable points[]*/
void print_points() {
    assert(points);
    int i;
    printf("points: ");
    for (i=0; i<n; i++) {
        printf("[%d,%df] ", points[i].x, points[i].y);
    }
    printf("\n");
    fflush(stdout);  //flush stdout, weird sync happens when using gl thread
}


/* ****************************** */
int main(int argc, char** argv) {
    // read number of points from user
    if (argc!=2) {
        printf("usage: viewPoints <nbPoints>\n");
        exit(1);
    }

    n = atoi(argv[1]);
    printf("you entered n=%d\n", n);
    assert(n >0);

    srand(time(NULL));
    reset();

    /* initialize GLUT  */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
    glutInitWindowPosition(100,100);
    glutCreateWindow(argv[0]);

    /* register callback functions */
    glutDisplayFunc(display);
    glutKeyboardFunc(keypress);

    /* init GL */
    /* set background color black*/
    glClearColor(0, 0, 0, 0);

    /* circular points */
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPointSize(POINT_SIZE);

    /* give control to event handler */
    glutMainLoop();
    return 0;
}

/* ****************************** */
/* draw a rectangle */
void draw_rectangle(int top, int bottom, int left, int right){
  glBegin(GL_QUADS);   //We want to draw a quad, i.e. shape with four sides
  glVertex2f(left, bottom);   //Draw the four corners of the rectangle
  glVertex2f(left, top);
  glVertex2f(right, top);
  glVertex2f(right, bottom);
  glEnd();
}

/* ****************************** */
/* draw a single point */
void draw_point(point2D point)
{
    glColor3fv(yellow);

    glBegin(GL_POINTS);
    glVertex2f(point.x, point.y);
    glEnd();
}

/* ****************************** */
/* draw a line between two points */
void draw_line(lineSegment2D line)
{
  glColor3fv(cyan);

  glBegin(GL_LINES);
  glVertex2f(line.p1.x, line.p1.y);
  glVertex2f(line.p2.x, line.p2.y);
  glEnd();
}

/* ****************************** */
/* draw the array of points stored in global variable points[]
   each point is drawn as a small square

*/
void draw_points(){

  const int R= 1;
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  //set color
  glColor3fv(yellow);

  assert(points);
  int i;
  for (i=0; i<n; i++) {
    draw_point(points[i]);
  }
}

/* ****************************** */
/* recursive draw function for drawing a tree rooted at the given node
 */
void draw_node(treeNode *node) {
  if (node == NULL) return;

  //if we are here, node must be valid

  //recursively draw the left node
  draw_node(node->left);

  //draw current node
  lineSegment2D l;
  point2D p; //dummy point, to create lines

  int margin = 2;
  int top = node->region.top - margin;
  int bottom = node->region.bot + margin;
  int left = node->region.left + margin;
  int right = node->region.right - margin;

  //for mondrian painting render
  int picker;

  switch (node->type) {
  case horiz:
    p = node->p;

    p.x = node->region.left; // left bound
    l.p1 = p;

    p.x = node->region.right; // right bound
    l.p2 = p;

    /* draw_line(l); */
    break;

  case vert:
    p = node->p;

    p.y = node->region.bot; // left bound
    l.p1 = p;

    p.y = node->region.top; // right bound
    l.p2 = p;

    /* draw_line(l); */
    break;

  case point:
    picker = rand() % 6;

    if(picker == 0) glColor3fv(red);
    else if(picker == 1) glColor3fv(blue);
    else if(picker == 2) glColor3fv(yellow);
    else glColor3fv(white);

    draw_rectangle(top, bottom, left, right);
    break;
  };

  //recurisvely draw right node
  draw_node(node->right);
}

/* ****************************** */
/* draw the kd-tree stored in the global variable kdTree
 */
void draw_kdtree()
{
    assert(tree);
    draw_node(tree->root);
}

/* ****************************** */
void display(void) {

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); //clear the matrix

    /* the default GL window is [-1,1]x[-1,1] with the origin in the
     center the points are in the range (0,0) to (WINSIZE,WINSIZE), so
     they need to be mapped to [-1,1]x [-1,1] */
    glScalef(2.0/WINDOWSIZE, 2.0/WINDOWSIZE, 1.0);
    glTranslatef(-WINDOWSIZE/2, -WINDOWSIZE/2, 0);

    draw_kdtree();

    /* execute the drawing commands */
    glFlush();
}

void reset() {
  //re-initialize points
  if (points) free(points);
  initialize_points();

  //free current tree
  if (tree) kdtree_free(tree);

  Rtimer rt1;

  rt_start(rt1);

  qsort(points, n, sizeof(point2D), xcomp);
  point2D *xpoints = points_cpy(points, n);
  qsort(points, n, sizeof(point2D), ycomp);
  point2D *ypoints = points_cpy(points, n);

  rect2D region;
  region.top = WINDOWSIZE;
  region.bot = 0;
  region.left = 0;
  region.right = WINDOWSIZE;

  tree = kdtree_build(xpoints, ypoints, region, n);
  rt_stop(rt1);

  char buf [1024];
  rt_sprint(buf,rt1);
  printf("time to generate kd-tree:  %s\n\n", buf);
  fflush(stdout);

  // print the tree
  //kdtree_print(tree);
  draw_kdtree();


  //free x/y point arrays
  if (xpoints) free(xpoints);
  if (ypoints) free(ypoints);
}

/* ****************************** */
void keypress(unsigned char key, int x, int y) {
    switch(key)
    {
        case ' ':
            reset();
            glutPostRedisplay();
            break;

        case 'q':
            exit(0);
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
