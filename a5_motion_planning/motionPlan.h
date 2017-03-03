#ifndef __motionPlan_h
#define __motionPlan_h

#include <vector>
#include "geom.h"

using namespace std;

// note: extract this to another class? maybe.
struct Node {
  point2D point;
  vector<Node*> children;
  vector<double> weights;

  //for Dijkstra's
  Node *parent;
  int shortest_distance;

  Node() { }
  Node(point2D p) { point = p; }
};

// returns true if l's distance from the start node is less than r's
struct Node_dist_comp{
  bool operator()(Node *l, Node *r) {
    // haha bug: in the future. think about what thing should have higher priority
    return(l->shortest_distance > r->shortest_distance);
  }
};

struct graze {
  point2D vert;
  int index;
  bool left;
};

/* returns a 2-vector of the start node and the end node, that defines
   the roadmap and shortest path */
vector<Node*> find_shortest_path(point2D start, point2D end, vector<vector<point2D> > poly_set);

// run dijkstra's algorithm to find shortest path from start to end
void dijkstras(Node *start, Node *end, vector<vector<Node*> > node_set);

// for the 'visited' list in dijkstras
// returns true if the Node a is in the vector nodes
bool contains_node(Node *a, vector<Node*> nodes);

// checks that all polygons are simple, and that neither point is in any of them
bool all_valid_polys(point2D start, point2D end, vector<vector<point2D> > poly_set);

// creates a 2D vector of nodes given a 2D vector of points
vector<vector<Node*> > build_node_set(vector<vector<point2D> > poly_set);

// builds a graph that connects all visible points to each other
void build_graph(Node *start_node, Node *end_node,
		 vector<vector<Node*> > node_set, vector<vector<point2D> > poly_set);

// frees all the nodes in the graph
void free_nodes(Node *node);

// connects a parent node to a child node, and adds edge weight as well
void connect(Node *parent, Node *child);

// returns ture if two verticies in the polygon set are visible, flase if not.
bool is_visible(point2D a, point2D b, vector<vector<point2D> > poly_set, int poly_index);

// returns true if polygon is simple
bool is_simple(vector<point2D> poly);

// returns true if the point is in the polygon
bool point_in_poly(point2D point, vector<point2D> poly);

// given an improper intersection between an segment and an edge,
// returns the index of the vertex that intersects improperly
int id_improp_index(point2D guard, point2D vert, int index, vector<point2D> poly);

// determine weather point is a 'graze point' with visibility line
graze is_graze(point2D guard, point2D vert, int vertIndex, vector<point2D> poly);

// returns the next vertex: "loops" around the vector if at end
point2D next_vertex(int i, vector<point2D> poly);

// inverse of next_vertex
point2D prev_vertex(int i, vector<point2D> poly);

// returns next index: "loops" around the vector if at end
// necessary for the identifying the
int next_index(int i, vector<point2D> poly);

#endif
