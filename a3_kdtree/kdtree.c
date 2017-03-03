/*

Laura Toma, based on code by John Visentin

*/

#include "kdtree.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

/* returns the root node of the given tree */
treeNode* kdtree_getRoot(kdtree *tree) {
  assert(tree);
  return tree->root;
}

/* returns the point stored by the node */
point2D treeNode_getPoint(treeNode *node) {

  assert(node);
  return node->p;
}

/* create a new empty tree */
kdtree* kdtree_init() {
  kdtree* tree = malloc(sizeof(kdtree));
  assert(tree);

  tree->root = NULL;
  tree->count = tree->height = 0;
  return tree;
}

/* private function to recursively print a node and its subtree in
   order */
static void treeNode_print(treeNode * node) {

  if (node == NULL) return;

  //if we are here, node must be valid

  //recursively print left child
  treeNode_print(node->left);

  //print node
  switch (node->type) {
  case horiz:
    printf("HORIZONTAL: (y=%d)\n", node->p.y);
    break;
  case vert:
    printf("VERTICAL: (x=%d)\n", node->p.x);
    break;
  case point:
    printf("LEAF: (p=(%d,%d))\n", node->p.x, node->p.y);
    break;
  default:
    printf("Improper tree node type\n");
    exit(1);
  };

  //recursively print right child
  treeNode_print(node->right);
}

/* print out information about the tree including height, number of
 nodes, and each node in an in-order traversal */
void kdtree_print(kdtree *tree) {
  if (tree) {
    printf("--- kdtree Info ---\n");
    printf("Height: %lu, Node Count: %lu\n", tree->height, tree->count);
    printf("Nodes in order:\n\n");
    treeNode_print(tree->root);
    printf("---------------------\n");
  }
}

//private function to recursively free the subtree rooted at node
static void treeNode_free(treeNode* node) {
  //  printf("about to try to free a node! \n");
  if(!node) return;
  if(node->left) treeNode_free(node->left);
  if(node->right) treeNode_free(node->right);
  free(node);
}

/* free all memory allocated for the tree, including the tree
   itself */
void kdtree_free(kdtree *tree) {
  if (!tree) return;
  treeNode_free(tree->root);
  free(tree);
}

// Comparator function for qsort. Sorts points by x values, then by y
int xcomp(const void* a, const void* b) {
  int ax = ((point2D *)a)->x;
  int ay = ((point2D *)a)->y;
  int bx = ((point2D *)b)->x;
  int by = ((point2D *)b)->y;

  return((ax > bx) || (ax == bx && ay > by));
}

// Comparator function for qsort. Sorts points by y values, then by x
int ycomp(const void* a, const void* b) {
  int ax = ((point2D *)a)->x;
  int ay = ((point2D *)a)->y;
  int bx = ((point2D *)b)->x;
  int by = ((point2D *)b)->y;

  return((ay > by) || (ay == by && ax > bx));
}

/* Splits given points array evenly into two arrays left and right*/
void splitpoints(point2D *points,point2D *left, point2D *right, int n, int mid) {
  int i;
  for(i=0; i<n; i++) {
    if(i <= mid) left[i] = points[i];
    if(i > mid) right[i-mid-1] = points[i];
  }
}

/* Given an array of points sorted, splits the array into two arrays
   based on their position relative to the Y VALUE of the median
   point*/
void distribute_y_points(point2D *ypoints, point2D *yleft, point2D *yright, int n, int mid, point2D median) {
  int i;
  int left_counter = 0;
  int right_counter = 0;
  for(i=0; i<n; i++) {
    if(xcomp(&ypoints[i], &median) < 1) {
      yleft[left_counter] = ypoints[i];
      left_counter++;
    }
    else {
      yright[right_counter] = ypoints[i];
      right_counter++;
    }
  }
}

/* Given an array of points sorted, splits the array into two arrays
   based on their position relative to the X VALUE of the median
   point*/
void distribute_x_points(point2D *xpoints, point2D *xleft, point2D *xright, int n, int mid, point2D median) {
  int i;
  int left_counter = 0;
  int right_counter = 0;
  for(i=0; i<n; i++) {
    if(ycomp(&xpoints[i], &median) < 1) {
      xleft[left_counter] = xpoints[i];
      left_counter++;
    }
    else {
      xright[right_counter] = xpoints[i];
      right_counter++;
    }
  }
}

/* Given an array of points, builds the kd tree recursively */
treeNode *grow_tree(treeNode *parent, point2D *xpoints, point2D *ypoints, rect2D region, int n) {
  //Base Case: No points left
  if(n == 0) {
    return NULL;
  }

  //if there ARE points left, create a node
  treeNode* node = malloc(sizeof(treeNode));
  assert(node);

  //Base case: only one point left
  if(n == 1) {
    node->p = xpoints[0];
    node->region = region;
    node->type = point;
    node->parent = parent;

    node->left = NULL;
    node->right = NULL;
  }

  //Recursive part
  else {
    // both horiz and vert nodes need this info
    int mid = (n-1)/2; //INDEX of median value
    int lsize = mid + 1; //SIZE of left point set

    node->region = region;
    node->parent = parent;

    //four arrays for splitting into left and right recursive calls
    point2D *xleft = malloc(n * sizeof(point2D));
    point2D *xright = malloc(n * sizeof(point2D));
    point2D *yleft = malloc(n * sizeof(point2D));
    point2D *yright = malloc(n * sizeof(point2D));

    // horiz and vert nodes have different info, need two if statements
    if(parent == NULL || parent->type == horiz) { //make VERT node
      node->p = xpoints[mid];
      node->type = vert;

      //split x point set into left and right sets
      splitpoints(xpoints, xleft, xright, n, mid);

      //build the two y sets based on the x split point
      distribute_y_points(ypoints, yleft, yright, n, mid, xpoints[mid]);

      rect2D l_reg = buildregion(region.top, region.bot, region.left, xpoints[mid].x);
      rect2D r_reg = buildregion(region.top, region.bot, xpoints[mid].x, region.right);

      node->left = grow_tree(node, xleft, yleft, l_reg, lsize);
      node->right = grow_tree(node, xright, yright, r_reg, n-lsize);
    }
    else if(parent->type == vert) { //make HORIZ node
      node->p = ypoints[mid];
      node->type = horiz;

      //split y point set into left and right sets
      splitpoints(ypoints, yleft, yright, n, mid);

      //build the two x sets based on the y split point
      distribute_x_points(xpoints, xleft, xright, n, mid, ypoints[mid]);

      rect2D l_reg = buildregion(ypoints[mid].y, region.bot, region.left, region.right);
      rect2D r_reg = buildregion(region.top, ypoints[mid].y, region.left, region.right);

      node->left = grow_tree(node, xleft, yleft, l_reg, lsize);
      node->right = grow_tree(node, xright, yright, r_reg, n-lsize);
    }

    // free the four arrays
    free(xleft);
    xleft = NULL;
    free(xright);
    xright = NULL;
    free(xright);
    xright = NULL;
    free(yright);
    yright = NULL;
  }//End else (end recursive section)

  return node;
}

/* create a new tree representing the given array of points */
//TODO: Maybe change this header to just have points and split to x and y here??
kdtree* kdtree_build(point2D *xpoints, point2D *ypoints, rect2D region, int n) {
  kdtree* tree = kdtree_init();
  tree->root = grow_tree(NULL, xpoints, ypoints, region, n);

  return tree;
}
