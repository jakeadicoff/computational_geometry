#include "geom.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <algorithm> // std::sort

/* returns the signed area of triangle abc. The area is positive if c
   is to the left of ab, and negative if c is to the right of ab
*/
int signed_area2D(point2D a, point2D b, point2D c) {
  return (b.x-a.x)*(c.y-a.y)-(c.x-a.x)*(b.y-a.y);
}

/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(point2D p, point2D q, point2D r) {
  return signed_area2D(p,q,r) == 0;
}

/* return 1 if c is  strictly left of ab; 0 otherwise */
int left (point2D a, point2D b, point2D c) {
  return signed_area2D(a,b,c) > 0;
}

/* returns all intersecting points in a set of orthog. line segments*/
vector<point2D> orthointersect_search(vector<segment2D> segments) {
  vector<point2D> intpoints;

  //put segement x-coordinate events from segments in a vector:
  //2 for horizontal start/end, 1 for vertical line
  vector<event> events = get_events(&segments);

  //sort vector of events by x_position
  //TODO: write event_comp: http://www.cplusplus.com/reference/algorithm/sort/
  sort(events.begin(), events.end(), event_comp);

  //find intersecting points, by traversing the event list
  intpoints = get_intersects(segments, events);
  cout << "Found " << intpoints.size() << " intersections" << endl;
  return intpoints;
}

/* returns all events from a given vector of segments: 2 events per
   horizontal line (start/end x-val) and 1 per vertical line (x-val)*/
vector<event> get_events(vector <segment2D> *segments) {
  vector<event> events;
  int xi, xf;
  event e;

  for(int i = 0; i < segments->size(); i++) {
    if ((*segments)[i].is_horiz()) {
      //add x values in left->right order
      if((*segments)[i].start.x < (*segments)[i].end.x) {
	xi = (*segments)[i].start.x;
	xf = (*segments)[i].end.x;
      } else {
	xf = (*segments)[i].start.x;
	xi = (*segments)[i].end.x;
      }

      //two events for horiz: start and end
      e.segment = &((*segments)[i]);
      e.x = xi;
      e.etype = h_start;
      events.push_back(e);

      e.x = xf;
      e.etype = h_end;
      events.push_back(e);
    } else {
      //one event for vert
      e.segment = &((*segments)[i]);
      e.x = (*segments)[i].start.x;
      e.etype = vert;
      events.push_back(e);
    }
  }

  return events;
}

/* find segment intersections using the ordered event list */
vector<point2D> get_intersects(vector<segment2D> segments, vector<event> events) {
  vector<point2D> intpoints;
  vector<point2D> new_ints;

  //create an active structure:
  //Set of segments, w/ segment's y position as comparator
  set<segment2D, bool(*)(segment2D, segment2D)> active_segs(horiz_seg_comp);

  //walk through list of events, adding intersections to a vector:
  //if horiz, add/remove from AS. if vert, check for intersects
  for(int i = 0; i < events.size(); i++) {
    switch (events[i].etype) {
    case h_start:
      active_segs.insert(*events[i].segment);
      break;
    case h_end:
      active_segs.erase(*events[i].segment);
      break;
    case vert:
      segment2D s = *events[i].segment;
      new_ints = vertrangesearch(active_segs, s.start.y, s.end.y, s.start.x);
      intpoints.insert(intpoints.end(), new_ints.begin(), new_ints.end());
      break;
    }
  }

  return intpoints;
}

/* finds all segments within a yi, yf range, and returns a vertex of
   intersection points */
vector<point2D> vertrangesearch(set<segment2D, bool(*)(segment2D, segment2D)> active_segs,
				int yi, int yf, int x) {
  vector<point2D> intpoints;
  point2D p;
  segment2D lower_dummy; //for set::iterator functions
  segment2D upper_dummy;

  p.x = x;

  //ensure correct y-ordering
  if(yi > yf) {
    int temp = yi;
    yi = yf;
    yf = temp;
  }

  lower_dummy.start.y = yi;
  upper_dummy.start.y = yf;

  set<segment2D, bool(*)(segment2D, segment2D)>::iterator begin = active_segs.lower_bound(lower_dummy);
  set<segment2D, bool(*)(segment2D, segment2D)>::iterator end = active_segs.upper_bound(upper_dummy);

  //iterate through all elements in set range
  for(set<segment2D, bool(*)(segment2D, segment2D)>::iterator it=begin; it!=end; ++it) {
    if(it->start.y >= yi && it->start.y <= yf) {
      p.y = it->start.y;
      intpoints.push_back(p);
    }
  }
  return intpoints;
}

/* event comparison, primarily by event.x value, secondarily by
   h_start, then vert, then h_end (for event list)*/
bool event_comp(event left, event right) {
  return(left.x < right.x || (left.x == right.x && left.etype < right.etype));
}

/* HORIZONTAL segment comparison, by segment's y value (for active
   structure)*/
bool horiz_seg_comp(segment2D left, segment2D right) {
  return(left.start.y < right.start.y);
}
