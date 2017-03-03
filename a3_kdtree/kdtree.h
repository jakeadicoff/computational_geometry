/*
   LT based on code from John Visentin
*/

#ifndef __kdtree_h
#define __kdtree_h

#include "geom.h"
#include <stddef.h> //for size_t

typedef enum _nodeType { horiz, vert, point } nodeType;

typedef struct _treeNode treeNode;

struct _treeNode {
  point2D p; /* If this is a leaf node, p represents the point
		stored in this leaf.  If this is not a leaf node, p
		represents the horizontal or vertical line stored in
		this node. For a vertical line, p.y is ignored. For
		a horizontal line, p.x is ignored
	     */

  rect2D region; // Records the region that the line/point is in

  nodeType type; // Can be horiz, vert, or point

  treeNode  *left, *right, *parent; /* left/below and right/above children. */
};

typedef struct _kdtree {
  treeNode *root;  //root of the tree
  size_t count; //number of nodes in the tree
  size_t height; //height of the tree
} kdtree;

/* returns the root node of the given tree */
treeNode* kdtree_getRoot(kdtree *tree);

/* returns the point stored by the node */
point2D treeNode_getPoint(treeNode *node);

/* create a new empty tree */
kdtree* kdtree_init();

// Comparator function for qsort. Sorts points by x values
int xcomp(const void* a, const void* b);

// Comparator function for qsort. Sorts points by y values
int ycomp(const void* a, const void* b);

/* create a new tree representing the given array of points */
kdtree* kdtree_build(point2D *xpoints, point2D *ypoints, rect2D region, int n);

/* free all memory allocated for the tree, including the tree
   itself */
void kdtree_free(kdtree *tree);

/* print out information about the tree including height, number of
 nodes, and each node in an in-order traversal */
void kdtree_print(kdtree *tree);

/* Splits given points array evenly into two arrays left and right*/
void splitpoints(point2D *points,point2D *left, point2D *right, int n, int mid);

/* Given an array of points sorted, splits the array into two arrays
   based on their position relative to the Y VALUE of the median
   point*/
void distribute_y_points(point2D *ypoints, point2D *yleft, point2D *yright, int n, int mid, point2D median);

/* Given an array of points sorted, splits the array into two arrays
   based on their position relative to the X VALUE of the median
   point*/
void distribute_x_points(point2D *xpoints, point2D *xleft, point2D *xright, int n, int mid, point2D median);

/* Given an array of points, builds the kd tree recursively */
treeNode *grow_tree(treeNode *parent, point2D *xpoints, point2D *ypoints, rect2D region, int n);

#endif
