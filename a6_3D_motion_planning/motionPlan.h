#ifndef __motionPlan_h
#define __motionPlan_h

#include <vector>
#include "geom.h"

using namespace std;

// note: extract this to another class? maybe.
struct Position {
  int x, y, t;

  double weight; // for A* search. Sum of euclidian distance from start
	      // and twice the euclidian distance from end
  Position *parent; // for A* (is this right???)

  bool operator == (const Position &p) {
    return(x==p.x && y== p.y && t== p.t);
  }

  Position() {}

Position(int xx, int yy, int tt, double ww): x(xx), y(yy), t(tt), weight(ww) {}

};



// for A* algorithm's priority queue
struct Weight_Comparator{
  // LOWER values are HIGHER priority
  bool operator()(Position l, Position r) {
    return(l.weight > r.weight);
  }
};


// for is_graze function
struct graze {
  point2D vert;
  int index;
  bool left;
};

/* returns a 2-vector of the start node and the end node, that defines
   the roadmap and shortest path */
vector<Position> find_shortest_path(vector<point2D> start_robot, vector<point2D> end_robot, vector<vector<point2D> > poly_set);

// run A* algorithm to find shortest path from start to end
vector<Position> a_star(vector<point2D> start_robot, vector<point2D> end_robot, vector<vector<point2D> > poly_set);

//
vector<Position> extract_path(Position start, Position end);

// given x y and theta, rotates and translates the robot to that location in parametric space
vector<point2D> place_bot(int x, int y, int t, vector<point2D> bot);

// given a robot in specific location in parametric space, returns true if robot is in free space
bool valid_bot_pos(vector<point2D> bot, vector<vector<point2D> > poly_set);

// checks that all polygons are simple, and that neither point is in any of them
bool all_valid_polys(vector<point2D> start, vector<point2D> end, vector<vector<point2D> > poly_set);

// returns true if polygon is simple
bool is_simple(vector<point2D> poly);

// returns true if polygons overlap properly
bool overlaps(vector<point2D> poly, vector<point2D> nextpoly);

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
int next_index(int i, int vecsize);

#endif
