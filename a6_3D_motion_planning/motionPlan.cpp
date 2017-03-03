#include <stdio.h>
#include <iostream>
#include <queue>
#include "motionPlan.h"
#include "geom.h"
#include <math.h>

using namespace std;


bool visited[250][250][90] = {{{false}}};
bool inPQ[250][250][90] = {{{false}}};
Position parents[250][250][90];


vector <Position> find_shortest_path(vector<point2D> start_robot, vector<point2D> end_robot, vector<vector<point2D> > poly_set) {
  vector <Position> path;

  if(all_valid_polys(start_robot, end_robot, poly_set) == false) {
    cout << "ERROR: INVALID POLYGON SET OR BAD ROBOT POSITIONING" << endl;
    return path;
  }
  
   for(int i=0; i<250; i++) {
    for(int j=0; j<250; j++) {
      for(int k=0; k<90; k++) {
	visited[i][j][k] = false;
	inPQ[i][j][k] = false;
      }
    }
  }

  path = a_star(start_robot, end_robot, poly_set);

  return path;
}

// get path form parents array
vector<Position> extract_path(Position start, Position end) {
  vector<Position> path;
  Position curr = end;
  while(!(curr == start)) {
    path.push_back(curr);
    curr = parents[curr.x][curr.y][curr.t];
    
  }
  path.push_back(start);
  return path;
}  


// run dijkstra's algorithm to find shortest path from start to end
vector<Position> a_star(vector<point2D> start_robot, vector<point2D> end_robot, vector<vector<point2D> > poly_set) {
  vector <Position> path;
  
  priority_queue<Position, vector<Position>, Weight_Comparator> pq;
  Position start((int)(start_robot[0].x)/3,(int)(start_robot[0].y/3), 0,
		 2*distance_formula(start_robot[0], end_robot[0]));  
  pq.push(start);
  Position end((int)(end_robot[0].x/3),(int)(end_robot[0].y/3), 0, 0);



  while(!pq.empty()) {
    Position curr_pos = pq.top();
    pq.pop();
    visited[curr_pos.x][curr_pos.y][curr_pos.t] = true;
    
    if(curr_pos == end) {
      cout << "found the end, yo!" << endl;
      break;
    }

    for(int dx=-1; dx<2; dx++) {
      if(curr_pos.x + dx >= 250 || curr_pos.x + dx < 0) continue;
      for(int dy=-1; dy<2; dy++) {
	if(curr_pos.y + dy >= 250 || curr_pos.y + dy < 0) continue;
	for(int dt=-1; dt<2; dt++) {

	  if(dx==0 && dx==0 && dt==0) continue;
	  int new_x, new_y, new_t;
	  new_x = curr_pos.x+dx;
	  new_y = curr_pos.y+dy;
	  new_t = curr_pos.t+dt;
	  
	  if(new_t >= 90) {
	    new_t = 0;
	  } else if(new_t < 0) {
	    new_t = 89;
	  }
	  
	  if(inPQ[new_x][new_y][new_t] == true) continue;
	  inPQ[new_x][new_y][new_t] = true;
	  if(visited[new_x][new_y][new_t] == true) continue;
	  vector <point2D> bot = place_bot(new_x*3, new_y*3, new_t*4, start_robot);
	  if(valid_bot_pos(bot, poly_set)) {
	    
	    point2D curr_point = {new_x*3, new_y*3};
	    double weight = 4*distance_formula(end_robot[0], curr_point) 
	      + 2*distance_formula(start_robot[0], curr_point) + abs(curr_pos.t - new_t);   	  
	 
	    Position p(new_x, new_y, new_t, weight);
	    pq.push(p);
	    parents[new_x][new_y][new_t] = curr_pos;
	 
	  } // close if

	} // close dt loop
      } // close dy loop
    } // close dx loop
  } // close while
	  
  path = extract_path(start, end);
  return path;
}

// generates a copy of the bot in the specified x,y,theta coordinates
vector<point2D> place_bot(int x, int y, int t, vector<point2D> bot) {
  double PI = 3.14159265;

  //distance from robots center to target position
  double dx = x - bot[0].x;
  double dy = y - bot[0].y;

  // define pivot point for robot tor rotate around
  double px = bot[0].x;
  double py = bot[0].y;

  // set values for sin and cos given angle t
  double s = sin(t*PI/180);
  double c = cos(t*PI/180);

  // applies rotation matrix on all points in the bot
  for(int i=0; i<bot.size(); i++) {
    bot[i].x -= px;
    bot[i].y -= py;

    double xNew = bot[i].x*c - bot[i].y*s;
    double yNew = bot[i].x*s + bot[i].y*c;

    bot[i].x = xNew + px + dx;
    bot[i].y = yNew + py + dy;
  }

  return bot;
}


bool valid_bot_pos(vector<point2D> bot, vector<vector<point2D> > poly_set) {
  for(int i=0; i<bot.size(); i++) {
    double xval = bot[i].x;
    double yval = bot[i].y;
    if(xval > 750 || xval < 0 || yval > 750 || yval < 0) return false;
  }

  for(int i=0; i<poly_set.size(); i++) {
    vector<point2D> poly = poly_set[i];

    // check if the start/end robot positions overlap with any polygon
    if(overlaps(bot, poly))
      return false;
  }

  return true;
}

// checks that all polygons are simple, and that neither point is in any of them
bool all_valid_polys(vector<point2D> start, vector<point2D> end, vector<vector<point2D> > poly_set) {
  // first check all the polygons
  for(int i=0; i<poly_set.size(); i++) {
    vector<point2D> poly = poly_set[i];

    // check every polygon for simplicity
    if(is_simple(poly) == false)
      return false;

    // check every two polygons for intersection
    for(int j=i+1; j<poly_set.size(); j++)
	if(overlaps(poly, poly_set[j]))
	  return false;
  }
  // now check the two robots

  // check if the start/end robot positions overlap with any polygon
  if(valid_bot_pos(start, poly_set) == false ||
     valid_bot_pos(end, poly_set) == false)
    return false;

  // if all checks pass, its a valid setup
  return true;
}

/* All of the following methods are from vispoly */
// returns true if polygon is simple
bool is_simple(vector<point2D> poly) {
  // check each edge with all other edges.
  // any intersection means its NOT a simple polygon
  for(int i = 0; i < poly.size(); i++) {
    point2D a = poly[i];
    point2D b = next_vertex(i, poly);

    for(int j = 0; j < poly.size(); j++) {
      point2D c = poly[j];
      point2D d = next_vertex(j, poly);

      // neighboring edges obviously intersect. We don't care
      if((a == c) || (a == d) || (b == c) || (b == d)) continue;
      if(intersect(a,b,c,d) == true) return false;
    }
  }

  // if no intersections found, it IS a simple polygon
  return true;
}

// returns true if the two polygons intersect each other
bool overlaps(vector<point2D> poly, vector<point2D> nextpoly) {
  // pick an edge from poly
  for(int i=0; i<poly.size(); i++) {
    point2D a = poly[i];
    point2D b = next_vertex(i, poly);

    // pick another edge from nextpoly
    for(int j=0; j<nextpoly.size(); j++){
      point2D c = nextpoly[j];
      point2D d = next_vertex(j, nextpoly);

      if(prop_intersect(a,b,c,d)) return true;
    }
  }

  return false;
}

// returns true if the point is in the polygon. Uses ray intersection
// counting method
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

      if(is_graze(point,poly[x],x,poly).vert.x != -1) {
	num_intersects++;
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

  // check for colinearity in two points surrounding vert
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
int next_index(int i, int vecsize) {
  if(i == vecsize-1) return 0;
  else return i+1;
}
