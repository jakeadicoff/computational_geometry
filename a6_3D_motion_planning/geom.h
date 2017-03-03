#ifndef __geom_h
#define __geom_h

using namespace std;

typedef struct _point2d {
  double x,y;

  bool operator == (const _point2d &p)
  {
    return (x == p.x && y == p.y);
  }
} point2D;

typedef struct _lineSegment2D {
    point2D p1, p2;
} lineSegment2D;

typedef struct _rect2D  {
    point2D origin;
    float width, height;
} rect2D;

/* returns the signed area of triangle abc. The area is positive if c
   is to the left of ab, and negative if c is to the right of ab
 */
double signed_area2D(point2D a, point2D b, point2D c);

/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(point2D p, point2D q, point2D r);

/* return the euclidian distnace btwn two points */
double distance_formula(point2D a, point2D b);

/* return 1 if c is strictly left of ab,
   return 0 if c is ON ab
   return -1 if c is RIGHT of ab */
int left(point2D a, point2D b, point2D c);

/* proper intersection test: lines ab and cd */
bool prop_intersect(point2D a, point2D b, point2D c, point2D d);

/* betweenness test, for c beween ab */
bool between(point2D a, point2D b, point2D c);

/* intersection test (proper AND improper): lines ab and cd */
bool intersect(point2D a, point2D b, point2D c, point2D d);

/* Exlusive or: true iff exactly one arg is true */
bool Xor(bool x, bool y);

/* returns approx intersection point among two line segments*/
point2D segSegInt(point2D a, point2D b, point2D c, point2D d);

/* retruns a point on the midpoint of a line*/
point2D midpoint(point2D a, point2D b);

#endif
