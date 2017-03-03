#ifndef __vispoly_h
#define __vispoly_h

#include <set>
#include <vector>
#include "geom.h"

using namespace std;

/* Takes a (simple) polygon and a point, and calculates the visible
   polygon from that point */
vector<point2D> find_vispoly(point2D guard, vector<point2D> poly);

// Checks if a polygon is simple
bool is_simple(vector<point2D> poly);

// returns the next vertex: "loops" around the vector if at end
point2D next_vertex(int i, vector<point2D> poly);

typedef struct _graze {
  point2D vert;
  int index;
  bool left;
} graze;

vector<graze> find_grazes(point2D guard, vector<point2D> poly);
vector<point2D> adjust_poly(point2D guard, vector<point2D> poly, set<int> *moved);
vector<point2D> slide_verts(vector<graze> grazeList, point2D guard, vector<point2D> poly, set<int> *moved);
vector<point2D> find_vis_area(vector<point2D> poly, point2D guard, set<int> guarenteed_visible);
bool guard_in_poly(point2D guard, vector<point2D> poly);
int next_index(int i, vector<point2D> poly);
point2D prev_vertex(int i, vector<point2D> poly);
bool is_visible(point2D guard, point2D vert, int vertIndex, vector<point2D> poly);
bool intersects_near_endpoint(point2D a, point2D b, point2D c, point2D d);
int id_improp_index(point2D guard, point2D vert, int index, vector<point2D> poly);
double distance_formula(point2D a, point2D b);
graze is_graze(point2D guard, point2D vert, int vertIndex, vector<point2D> poly);
bool in_graze_list(int index, vector<graze> grazeList, vector<point2D> poly);
#endif
