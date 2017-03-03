#ifndef __geom_h
#define __geom_h

#include <vector>

using namespace std;


typedef struct _point2d {
  int x,y;

  bool operator == (const _point2d &p)
  {
    return (x == p.x && y == p.y);
  }
} point2D;


/* returns 2 times the signed area of triangle abc. The area is
   positive if c is to the left of ab, and negative if c is to the
   right of ab
 */
int signed_area2D(point2D a, point2D b, point2D c);

/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(point2D p, point2D q, point2D r);

/* return 1 if c is  strictly left of ab; 0 otherwise */
int left (point2D a, point2D b, point2D c);

/* compute the convex hull of the points in p; the points on the CH
   are returned as a vector (using Andrew's Monotone Chain Scan)
*/
vector<point2D> andrew_scan(vector<point2D>);

/* returns vector of most extreme x points. If multiple l/r extremes
   exist, take the lowest left point and the highest right point.
*/
vector<point2D> get_midline(vector<point2D> p);

/* returns a vector containing two vectors: one w/ all points above
   the midline, and one w/ all points below the midline
 */
vector<vector <point2D> > separate_points(vector<point2D> p, vector<point2D> midline);

/* returns lexographically sorted vector of points above/below the
   midline, using a modified mergesort
 */
vector<point2D> upper_lexSort(vector<point2D> p);
vector<point2D> lower_lexSort(vector<point2D> p);

/* returns the upper and lower convex hulls
 */
vector<point2D> find_upper_hull(vector<point2D> upperHull);
vector<point2D> find_lower_hull(vector<point2D> points);

/* removes any consecutive duplicates in the point set. note: first
   point must be included twice to close the polygon
 */
vector<point2D> remove_duplicates(vector<point2D> points);
#endif
