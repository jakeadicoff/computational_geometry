#include "geom.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <vector>

using namespace std;

/* **************************************** */
/* returns the signed area of triangle abc. The area is positive if c
   is to the left of ab, and negative if c is to the right of ab
*/
int signed_area2D(point2D a, point2D b, point2D c) {
  return (b.x-a.x)*(c.y-a.y)-(c.x-a.x)*(b.y-a.y);
}

/* **************************************** */
/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(point2D p, point2D q, point2D r) {
  return signed_area2D(p,q,r) == 0;
}

/* **************************************** */
/* return 1 if c is  strictly left of ab; 0 otherwise */
int left (point2D a, point2D b, point2D c) {
  return signed_area2D(a,b,c) > 0;
}

/* compute the convex hull of the points in p; the points on the CH
   are returned as a vector (using Andrew's Monotone Chain Scan)
*/
vector<point2D> andrew_scan(vector<point2D> p) {
  //find the midline
  vector<point2D> midline = get_midline(p);
  //If the midline is vertical, it must be the hull
  if(midline[0].x == midline[1].x) {
    return midline;
  }

  //Separate points into above and below midline
  vector <vector <point2D> > sets = separate_points(p, midline);
  vector<point2D> upper_points = sets[0];
  vector<point2D> lower_points = sets[1];
  //add points from midline to both sets
  upper_points.push_back(midline[0]);
  upper_points.push_back(midline[1]);
  lower_points.push_back(midline[0]);
  lower_points.push_back(midline[1]);

  //sort upper and lower sets of points lexographically
  upper_points = upper_lexSort(upper_points);
  lower_points = lower_lexSort(lower_points);

  //find the respective hulls of both point sets
  vector<point2D> upper_hull = find_upper_hull(upper_points);
  vector<point2D> lower_hull = find_lower_hull(lower_points);

  //combines the two hulls (appends lower hull in reverse order)
  upper_hull.insert(upper_hull.end(),lower_hull.rbegin(),lower_hull.rend());

  //deletes any duplicates that may have been added
  upper_hull = remove_duplicates(upper_hull);

  return upper_hull;
}

/* returns vector of most extreme x points. If multiple l/r extremes
   exist, take the lowest left point and the highest right point.
*/
vector<point2D> get_midline(vector<point2D> p) {
  vector<point2D> midline;
  point2D min = p[0];
  point2D max = p[0];

  for(int i=0; i<p.size(); i++) {
    if(p[i].x < min.x || (p[i].x == min.x && p[i].y < min.y)) {
      min = p[i];
    }
    else if(p[i].x > max.x || (p[i].x == max.x && p[i].y > max.y)) {
      max = p[i];
    }
  }

  midline.push_back(min);
  midline.push_back(max);
  return midline;
}

/* returns a vector containing two vectors: one w/ all points above
   the midline, and one w/ all points below the midline
 */
vector<vector <point2D> > separate_points(vector<point2D> p, vector<point2D> midline) {
  vector<vector <point2D> > sets;
  vector<point2D> upper_points;
  vector<point2D> lower_points;

  for(int i = 0; i < p.size(); i++) {
    if(left(midline[0],midline[1],p[i]) == 1){
      upper_points.push_back(p[i]);
    }
    else {
      lower_points.push_back(p[i]);
    }
  }

  sets.push_back(upper_points);
  sets.push_back(lower_points);
  return sets;
}

/* returns lexographically sorted vector of points above the midline,
   using a modified mergesort. First by x-values, then by y
 */
vector<point2D> upper_lexSort(vector<point2D> p) {
  //base case
  if (p.size() < 2) {
    return p;
  }

  //split phase
  vector<point2D> rnew(p.begin()+p.size()/2,p.end());
  vector<point2D> right = upper_lexSort(rnew);
  vector<point2D> lnew(p.begin(),p.begin()+p.size()/2);
  vector<point2D> left = upper_lexSort(lnew);

  //merge phase
  vector<point2D> sorted;
  int l_iter = 0;
  int r_iter = 0;
  //merges by increasing x-values primarily, then by y-values
  while (l_iter < left.size() && r_iter < right.size()) {
    if (left[l_iter].x < right[r_iter].x ||
	(left[l_iter].x == right[r_iter].x && left[l_iter].y < right[r_iter].y)) {
      sorted.push_back(left[l_iter]);
      l_iter++;
    } else {
      sorted.push_back(right[r_iter]);
      r_iter++;
    }
  }
  if (l_iter < left.size()) {
    sorted.insert(sorted.end(),left.begin()+l_iter,left.end());
  } else{
    sorted.insert(sorted.end(),right.begin()+r_iter,right.end());
  }

  return sorted;
}

/* returns lexographically sorted vector of points above the midline,
   using a modified mergesort. First by x-values, then by y
 */
vector<point2D> lower_lexSort(vector<point2D> p) {
  //base case
  if (p.size() < 2) {
    return p;
  }

  //split phase
  vector<point2D> rnew(p.begin()+p.size()/2,p.end());
  vector<point2D> right = upper_lexSort(rnew);
  vector<point2D> lnew(p.begin(),p.begin()+p.size()/2);
  vector<point2D> left = upper_lexSort(lnew);

  //merge phase
  vector<point2D> sorted;
  int l_iter = 0;
  int r_iter = 0;
  while (l_iter < left.size() && r_iter < right.size()) {
    if (left[l_iter].x < right[r_iter].x ||
	(left[l_iter].x == right[r_iter].x && left[l_iter].y > right[r_iter].y)) {
      sorted.push_back(left[l_iter]);
      l_iter++;
    } else {
      sorted.push_back(right[r_iter]);
      r_iter++;
    }
  }
  if (l_iter < left.size()) {
    sorted.insert(sorted.end(),left.begin()+l_iter,left.end());
  } else{
    sorted.insert(sorted.end(),right.begin()+r_iter,right.end());
  }

  return sorted;
}

/* returns the upper convex hull
 */
vector<point2D> find_upper_hull(vector<point2D> points) {
  vector<point2D> hull;

  //add first two points
  hull.push_back(points[0]);
  hull.push_back(points[1]);

  //add points if convex, repeatedly pop last point if not convex
  for(int i = 2; i < points.size();i++) {
    //if next point is right of last two points & not collinear, add to hull
    if(left(hull[hull.size()-2], hull[hull.size()-1],points[i]) == 0 &&
       collinear(hull[hull.size()-2], hull[hull.size()-1],points[i]) == 0) {
      hull.push_back(points[i]);
    }
    //else, pop points off hull-so-far until the new point can be added
    else {
      do {
	hull.pop_back();
      } while (hull.size() > 1 &&
	       (left(hull[hull.size()-2], hull[hull.size()-1],points[i])==1 ||
		collinear(hull[hull.size()-2], hull[hull.size()-1], points[i])==1));
    }
    hull.push_back(points[i]);
  }

  return hull;
}

/* returns the lower convex hull
 */
vector<point2D> find_lower_hull(vector<point2D> points) {
  vector<point2D> hull;

  //add first two points
  hull.push_back(points[0]);
  hull.push_back(points[1]);

  //add points if convex, repeatedly pop last point if not convex
  for(int i = 2; i < points.size();i++) {
    if(left(hull[hull.size()-2], hull[hull.size()-1],points[i]) == 1 &&
       collinear(hull[hull.size()-2], hull[hull.size()-1],points[i]) == 0) {
      hull.push_back(points[i]);
    }
    else {
      do {
	hull.pop_back();
      } while (hull.size() > 1 &&
	       (left(hull[hull.size()-2], hull[hull.size()-1],points[i])==0 ||
		collinear(hull[hull.size()-2], hull[hull.size()-1],points[i])==1));
    }
    hull.push_back(points[i]);
  }

  return hull;
}

/* removes any consecutive duplicates in the point set. note: first
   point must be included twice to close the polygon
 */
vector<point2D> remove_duplicates(vector<point2D> points) {
  point2D first = points[0]; //because the point vector is "circular"
  point2D prev = points[0];

  for(int i=1; i < points.size()-1; i++) {
    if(points[i] == prev || points[i] == first) {
      points.erase(points.begin()+i);
      i--;
    }
    else {
      prev = points[i];
    }
  }

  return points;
}
