#ifndef __geom_h
#define __geom_h

#include <vector>
#include <set>

using namespace std;

typedef struct _point2d {
  int x,y;
} point2D;

typedef struct _segment2d {
  point2D start;
  point2D end;

  bool is_horiz() { return(start.y == end.y);}
} segment2D;

enum EventType { h_start, vert, h_end };
typedef struct _event{
  segment2D *segment;
  int x; // x position of event
  EventType etype;
} event;

/* returns the signed area of triangle abc. The area is positive if c
   is to the left of ab, and negative if c is to the right of ab
 */
int signed_area2D(point2D a, point2D b, point2D c);

/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(point2D p, point2D q, point2D r);

/* return 1 if c is  strictly left of ab; 0 otherwise */
int left (point2D a, point2D b, point2D c);

/* returns all intersecting points in a set of orthog. line segments*/
vector<point2D> orthointersect_search(vector<segment2D> segments);

/* returns all events from a given vector of segments: 2 events per
   horizontal line (start/end x-val) and 1 per vertical line (x-val)*/
vector<event> get_events(vector<segment2D> *segments);

/* find segment intersections using the ordered event list */
vector<point2D> get_intersects(vector<segment2D> segments, vector<event> events);

/* finds all segments within a yi, yf range, and returns a vertex of
   intersection points */
vector<point2D> vertrangesearch(set<segment2D, bool(*)(segment2D, segment2D)> active_segs,
				int yi, int yf, int x);
#endif

/* event comparison, primarily by event.x value, secondarily by
   h_start, then vert, then h_end */
bool event_comp(event left, event right);

/* HORIZONTAL segment comparison, by segment's y value */
bool horiz_seg_comp(segment2D left, segment2D right);
