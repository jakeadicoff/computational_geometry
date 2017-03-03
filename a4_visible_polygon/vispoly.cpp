#include "vispoly.h"
#include "geom.h"
#include <math.h>
#include <iostream>
#include <algorithm>

using namespace std;

// Main function: given a guard and a polygon, returns the visible polygon
vector<point2D> find_vispoly(point2D guard, vector<point2D> poly) {
  vector<point2D> vispoly;

  // check for simple polygon (ret empty polygon if not)
  if(is_simple(poly) == false) {
    cout << "Polygon is not simple! Draw a new polygon." << endl;
    return vispoly;
  }

  // check for point inside polygon (ret empty polgon if not)
  if(guard_in_poly(guard,poly) == false) {
    cout << "Guard is not in polygon! Move guard." << endl;
    return vispoly;
  }

  set<int> guarenteed_visible; //indecies of points that must be visible

  // method to find graze points and move invisible verticies 'behind' the grazes
  vispoly = adjust_poly(guard, poly, &guarenteed_visible);
  vispoly = find_vis_area(vispoly, guard, guarenteed_visible);
  return vispoly;
}

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

bool guard_in_poly(point2D guard, vector<point2D> poly) {
  point2D a;
  int num_intersects = 0;
  a.y = guard.y;
  a.x = guard.x + 2000;
  for(int i = 0; i < poly.size(); i++) {
    if(intersect(guard,a,poly[i],next_vertex(i,poly))) {
      if(prop_intersect(guard,a,poly[i],next_vertex(i,poly))) {
	num_intersects++;
	continue;
      }
      int x = id_improp_index(guard,poly[i],i,poly);
      if(is_graze(guard,poly[x],x,poly).vert.x != -1) {
	num_intersects++;
      }
    }
  }
  if(num_intersects%2 == 0) return false;
  return true;
}

// "adjusts" the polygon so behind each grazing intersection, there's
// a vertex for the visible polygon to find
vector<point2D> adjust_poly(point2D guard, vector<point2D> poly, set<int> *moved) {
  vector<graze> grazeList = find_grazes(guard, poly);
  poly = slide_verts(grazeList, guard, poly, moved);
  return poly;
}

// return a vector of all grazing vertecies from the guard's point of view
vector<graze> find_grazes(point2D guard, vector<point2D> poly) {
  vector<graze> grazeList;
  for(int i = 0; i < poly.size(); i++) {
    if(is_visible(guard, poly[i], i, poly)) {
      graze g = is_graze(guard, poly[i], i, poly);
	if(g.vert.x != -1) {
	grazeList.push_back(g); // CORRECT THIS DUMBASS, ITS A GRAZE OBJECT. NOW LOOK UP
      }
    }
  }
  cout << "size of graze list: " << grazeList.size() << endl;
  return grazeList;
}

// given a gaurd and a vertex, tells if a vertex is visible from gaurd. excludes grazes
bool is_visible(point2D guard, point2D vert, int vertIndex, vector<point2D> poly) {
  point2D a,b;
  for(int i = 0; i < poly.size(); i++) {
    a = poly[i];
    b = next_vertex(i, poly);
    if (a == vert || b == vert) continue; // CHECK THIS
    if(intersect(guard,vert,a,b)) {
      if(prop_intersect(guard,vert,a,b)) return false;
      int improp_index = id_improp_index(guard,vert,i,poly);
      if(is_graze(guard,poly[improp_index],improp_index,poly).vert.x != -1) return false;
    }
  }
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

//compute distance between two points in 2D
double distance_formula(point2D a, point2D b) {
  return (sqrt(pow(a.x-b.x,2)+pow(a.y-b.y,2)));
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
  cout << "WHAT THE ACTUAL FUCK!" << endl;
  return dummyGraze;
}

// change vertecies of the polygon so that they lie on the guard's
// line of sight
vector<point2D> slide_verts(vector<graze> grazeList, point2D guard, vector<point2D> poly, set<int> *moved) {
  int BIG_INT = 2000;
  for(int i = 0; i < grazeList.size(); i++) {
    int min_dist = BIG_INT; // initialize to impossible values
    int edge_index = -1;
    point2D a, closest_int;

    //draw our ray from our grazing segment out to infinity
    a.x = grazeList[i].vert.x + BIG_INT*(grazeList[i].vert.x - guard.x);
    a.y = grazeList[i].vert.y + BIG_INT*(grazeList[i].vert.y - guard.y);

    //compare our ray to all edges in the polygon
    for(int j = 0; j < poly.size(); j++) {
      if(grazeList[i].vert == poly[j] || grazeList[i].vert == next_vertex(j,poly)) continue;
      point2D intPoint;
      intPoint = segSegInt(guard, a, poly[j], next_vertex(j,poly));
      if(intPoint.x == -1) continue; //no intersection found
      if(distance_formula(guard,intPoint) < min_dist) {
	min_dist = distance_formula(guard,intPoint);
	closest_int = intPoint;
	edge_index = j;
      }
    }

    if(closest_int == poly[edge_index] || closest_int == next_vertex(edge_index, poly)) continue;

    //slide the appropriate vertex to our 'visible' spot
    if(grazeList[i].left == true) { // if left graze
      if(left(guard,a,poly[edge_index]) == 1) {
	poly[edge_index] = closest_int;
	moved->insert(edge_index);
      } else {
	int next_ind = next_index(edge_index,poly);
	poly[next_ind] = closest_int;
	moved->insert(next_ind);
      }
    } else if(grazeList[i].left == false) {
      if(left(guard,a,poly[edge_index]) == -1) {
	poly[edge_index] = closest_int;
	moved->insert(edge_index);
      } else {
	int next_ind = next_index(edge_index,poly);
	poly[next_ind] = closest_int;
	moved->insert(next_ind);
      }
    }
  }
  return poly;
}

// returns true if any edge (defined by index, index + 1) containz a point in the graze list
bool in_graze_list(int index, vector<graze> grazeList, vector<point2D> poly) {
  for(int i = 0; i < grazeList.size(); i++) {
    if(grazeList[i].vert == poly[index]) return true;
    if(grazeList[i].vert == poly[next_index(index,poly)]) return true;
  }
  return false;
}

// after the polygon is adjusted, walks the boundary and collects all visible vertecies
vector<point2D> find_vis_area(vector<point2D> poly, point2D guard, set<int> guarenteed_visible) {
  vector<point2D> vis_verts;

  for(int i = 0; i < poly.size(); i++) {
    if(guarenteed_visible.count(i) != 0) {
      vis_verts.push_back(poly[i]);
      cout << "visPoint: (" << poly[i].x << "," << poly[i].y << ")" << endl;
    }
    else if(is_visible(guard, poly[i], i, poly)) {
      vis_verts.push_back(poly[i]);
      cout << "visPoint: (" << poly[i].x << "," << poly[i].y << ")" << endl;
    }
  }
  return vis_verts;
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
int next_index(int i, vector<point2D> poly) {
  if(i == poly.size()-1) return 0;
  else return i+1;
}
