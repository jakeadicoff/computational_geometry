#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include "motionPlan.h"
#include "geom.h"

using namespace std;

vector<Node*> find_shortest_path(point2D start, point2D end, vector<vector<point2D> > poly_set) {
  vector<Node*> node_holder; // it holds mah nodes
  Node *start_node = new Node(start);
  Node *end_node = new Node(end);

  if(all_valid_polys(start, end, poly_set) == false) {
    cout << "ERROR: There's a complex polygon OR "
      "one of the points are inside of a polygon! \n"
      "Hit 'a' to restart" << endl;
    return node_holder;
  }

  vector<vector<Node*> > node_set = build_node_set(poly_set);

  build_graph(start_node, end_node, node_set, poly_set);

  dijkstras(start_node, end_node, node_set);

  node_holder.push_back(start_node);
  node_holder.push_back(end_node);

  return node_holder;
}

// run dijkstra's algorithm to find shortest path from start to end
void dijkstras(Node *start, Node *end, vector<vector<Node*> > node_set) {
  vector<Node*> visited;
  priority_queue<Node*, vector<Node*>, Node_dist_comp> pq;

  start->shortest_distance = 0;
  pq.push(start);

  for(int i=0; i<node_set.size(); i++) {
    for(int j=0; j<node_set[i].size(); j++) {
      node_set[i][j] -> shortest_distance = 1000000;
      pq.push(node_set[i][j]);
    }
  }

  end->shortest_distance = 1000000;
  pq.push(end);


  while(!pq.empty()) {
    Node *current = pq.top();
    pq.pop();

    if(current == end) break; // if we find the target point, we're done

    if(contains_node(current, visited)) continue;
    visited.push_back(current);

    for(int i = 0; i<current->children.size(); i++) {
      Node *child = current->children[i];

      int child_wgt = current->weights[i];
      int newdist = current->shortest_distance + child_wgt;

      if(newdist < child->shortest_distance) {
	child->shortest_distance = newdist;
	child->parent = current;

	pq.push(child);
      }
    }
  }
}

// returns true if the Node a is in the vector of nodes
bool contains_node(Node *a, vector<Node*> nodes) {
  for(int i=0; i<nodes.size(); i++)
    if(nodes[i] == a) return true;
  return false;
}


// builds a graph that connects all visible points to each other (ie a roadmap)
void build_graph(Node *start_node, Node *end_node, vector<vector<Node*> > node_set, vector<vector<point2D> > poly_set) {
  point2D start = start_node->point;
  point2D end = end_node->point;

  if(is_visible(start,end,poly_set, -1)) {
    connect(start_node, end_node);
    connect(end_node, start_node);
  }

  //grab a point 'a' and a point 'b'
  for(int i=0; i<node_set.size(); i++) {
    for(int j=0; j<node_set[i].size(); j++) {
      point2D a = node_set[i][j]->point;

      if(is_visible(start,a,poly_set,-1)) {
	connect(start_node, node_set[i][j]);
	connect(node_set[i][j], start_node);
      }

      if(is_visible(end,a,poly_set,-1)) {
	connect(end_node, node_set[i][j]);
	connect(node_set[i][j], end_node);
      }

      for(int k=0; k<node_set.size(); k++) {
	for(int l=0; l<node_set[k].size(); l++) {

	  if(k != i || l != j) { // make sure a and b are different
	    point2D b = node_set[k][l]->point;

	    // if A sees B, A is B's parent
	    if(is_visible(a,b,poly_set,k))
	      connect(node_set[i][j], node_set[k][l]);
	  }
	}
      }//inner double for loop
    }
  } //outer double for loop
}

// frees all the nodes in the graph
void free_nodes(Node *node) {
  if(node == NULL) return;

  vector<Node*> stack;
  stack.push_back(node);

  while(!stack.empty()) {
    cout << "in while loop" << endl;

    Node *current = stack.back();
    stack.pop_back();

    if(current == NULL) continue;


    // add all the undiscovered children to the stack
    for(int i =0; i<current->children.size(); i++) {
      cout << "SEGFAULT????????????????????????" << endl;
      Node *child = current->children[i];

      cout << "SEGFAULT twooooooooooooo ????????????????????????" << endl;
      cout << (child != NULL) << endl;
      cout << " the condition is ok" << endl;

      if(child != NULL) {
	cout << "child is: " << child << endl;
	cout << "about to push the child" << endl;
	stack.push_back(child);
      }
      cout << "SEGFAULT 4 ????????????????????????" << endl;
    }

    cout << "BE FREE" << endl;
    // free the current node
    free(current);
    current = NULL;
  }
}

// connects a parent node to a child node, and adds edge weight as well
void connect(Node *parent, Node *child) {
  double weight = distance_formula(parent->point, child->point);

  parent->children.push_back(child);
  parent->weights.push_back(weight);
}

// returns true if the two points have an unblocked line of sight
bool is_visible(point2D a, point2D b, vector<vector<point2D> > poly_set, int poly_index) {
  // first, check for any proper intersects
  for(int i = 0; i < poly_set.size(); i++) {
    for(int j = 0; j < poly_set[i].size(); j++) {
      point2D x = poly_set[i][j];
      point2D y = poly_set[i][next_index(j, poly_set[i])];
      if(prop_intersect(a,b,x,y)) return false;
    }
  }

  // then, check if line is a diagonal
  if(poly_index != -1) {
    point2D midPoint = midpoint(a,b);

    // offset the midpoint in 4 directions, to make sure midpoint is
    // REALLY inside polygon
    int epsilon = 1;
    point2D mp_n, mp_s, mp_e, mp_w;
    mp_n = mp_s = mp_e = mp_w = midPoint;
    mp_n.y = mp_n.y + epsilon;
    mp_s.y = mp_s.y - epsilon;
    mp_e.x = mp_e.x + epsilon;
    mp_w.x = mp_w.x - epsilon;

    if(point_in_poly(mp_n, poly_set[poly_index]) &&
       point_in_poly(mp_s, poly_set[poly_index]) &&
       point_in_poly(mp_e, poly_set[poly_index]) &&
       point_in_poly(mp_w, poly_set[poly_index])) return false;
  }

  // then, check for any 'piercing' intersects...
  for(int i = 0; i < poly_set.size(); i++) {
    int left_intersects = 0, right_intersects = 0;
    for(int j = 0; j < poly_set[i].size(); j++) {
      point2D x = poly_set[i][j];
      point2D y = poly_set[i][next_index(j, poly_set[i])];
      if(intersect(a,b,x,y)) {
	if(left(a,b,x) == 1 || left(a,b,y) == 1) left_intersects++;
	if(left(a,b,x) == -1 || left(a,b,y) == -1) right_intersects++;
      }
    }
    // ...that is, if we every have a 'left' and a 'right' peircing
    // intersect in the same polygon, we've peirced the polygon
    if(left_intersects > 1 && right_intersects > 1) return false;
  }

  return true;
}


// creates a 2D vector of nodes given a 2D vector of points
vector<vector<Node*> > build_node_set(vector<vector<point2D> > poly_set) {
  vector<vector<Node*> > node_set;

  for(int i=0; i<poly_set.size(); i++) {
    vector<Node*> temp;
    for(int j=0; j<poly_set[i].size(); j++) {
      Node *n = new Node(poly_set[i][j]);
      temp.push_back(n);
    }
    node_set.push_back(temp);
  }

  return node_set;
}

// checks that all polygons are simple, and that neither point is in any of them
bool all_valid_polys(point2D start, point2D end, vector<vector<point2D> > poly_set) {
  for(int i=0; i<poly_set.size(); i++) {
    vector<point2D> poly = poly_set[i];

    if(is_simple(poly) == false ||
       point_in_poly(start, poly) == true || point_in_poly(end, poly) == true)
      return false;
  }

  return true;
}

/* All of the following methods are from vispoly */
// returns true if polygon is simple
bool is_simple(vector<point2D> poly) {
  point2D a, b, c, d;

  // check each edge with all other edges. Any intersect means its NOT a simple polygon
  for(int i = 0; i < poly.size(); i++) {
    a = poly[i];
    b = next_vertex(i, poly);

    for(int j = 0; j < poly.size(); j++) {
      c = poly[j];
      d = next_vertex(j, poly);

      // neighboring edges obviously intersect. We don't care
      if((a == c) || (a == d) || (b == c) || (b == d)) continue;
      if(intersect(a,b,c,d) == true) return false;
    }
  }

  // if no intersections found, it IS a simple polygon
  return true;
}

// returns true if the point is in the polygon
bool point_in_poly(point2D point, vector<point2D> poly) {
  point2D a;
  int num_intersects = 0;
  a.y = point.y;
  a.x = point.x + 2000;
  for(int i = 0; i < poly.size(); i++) {
    if(intersect(point,a,poly[i],next_vertex(i,poly))) {
      if(prop_intersect(point,a,poly[i],next_vertex(i,poly))) {
	num_intersects++;
	continue;
      }

      int x = id_improp_index(point,poly[i],i,poly);

      if(x == i) {
	if(is_graze(point,poly[i],i,poly).vert.x != -1) {
	  num_intersects++;
	}
      } else if (x == next_index(i,poly)) {
	if(is_graze(point,poly[x],x,poly).vert.x != -1) {
	  num_intersects++;
	}
      } else{
	cout << "PROBLEM, YO" << endl;
      }
    }
  }
  if(num_intersects%2 == 0) return false;
  return true;
}

// given an improper intersection between an segment and an edge,
// returns the index of the vertex that intersects improperly
int id_improp_index(point2D guard, point2D vert, int index, vector<point2D> poly) {
  if(between(guard,vert,poly[index])) {
    return index;
  }
  if(between(guard,vert,next_vertex(index,poly))) {
    if(poly.size()==index+1) return 0;
    return index;
  }
  return -1;
}

// determine weather point is a 'graze point' with visibility line
graze is_graze(point2D guard, point2D vert, int vertIndex, vector<point2D> poly) {
  point2D dummyPoint;
  dummyPoint.x = -1;
  dummyPoint.y = -1;
  graze dummyGraze, g;
  dummyGraze.vert = dummyPoint;
  g.vert = vert;
  g.index = vertIndex;

  point2D vNext, vPrev;
  vNext = next_vertex(vertIndex, poly);
  vPrev = prev_vertex(vertIndex, poly);
  int iNext, iPrev;
  if(vertIndex == poly.size()-1) {
    iNext = 0;
  } else {
    iNext = vertIndex+1;
  }
  if(vertIndex == 0) {
    iPrev = poly.size()-1;
  } else {
    iPrev = vertIndex -1;
  }

  // check for cilinearity in two points surrounding vert
  int loc_next, loc_prev;
  loc_next = left(guard,vert,vNext);
  loc_prev = left(guard,vert,vPrev);

  // if no colinear points
  if(loc_next != 0 && loc_prev != 0) {
    if(loc_next != loc_prev) return dummyGraze;
    if(loc_next == 1) {
      g.left = true;
    } else {
      g.left = false;
    }
    return g;
  }

  if(loc_next == 0){
    if(loc_prev == 1) {
      g.left = true;
    } else {
      g.left = false;
    }
    point2D adv = next_vertex(iNext,poly);
    int loc_adv = left(guard,vert,adv);
    if(loc_prev == loc_adv) return g;
    if(distance_formula(guard,vert) < distance_formula(guard,vNext)) {
      return g;
    }
    return dummyGraze;
  }
  if(loc_prev == 0) {
    if(loc_next == 1) {
      g.left = true;
    } else {
      g.left = false;
    }
    point2D adv = prev_vertex(iPrev,poly);
    int loc_adv = left(guard,vert,adv);
    if(loc_next == loc_adv) return g;
    if(distance_formula(guard,vert) < distance_formula(guard,vPrev)) {
      return g;
    }
    return dummyGraze;
  }
  cout << "Error Error!" << endl;
  return dummyGraze;
}

// returns the next vertex: "loops" around the vector if at end
point2D next_vertex(int i, vector<point2D> poly) {
  if(i == poly.size()-1) return poly[0];
  else return poly[i+1];
}

// inverse of next_vertex
point2D prev_vertex(int i, vector<point2D> poly) {
  if(i == 0) return poly[poly.size()-1];
  else return poly[i-1];
}

// returns next index: "loops" around the vector if at end
// necessary for the identifying the
int next_index(int i, vector<point2D> poly) {
  if(i == poly.size()-1) return 0;
  else return i+1;
}
