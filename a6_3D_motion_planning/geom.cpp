#include "geom.h"
#include "math.h"
/* returns the signed area of triangle abc. The area is positive if c
   is to the left of ab, and negative if c is to the right of ab
*/
double signed_area2D(point2D a, point2D b, point2D c) {
  return (b.x-a.x)*(c.y-a.y)-(c.x-a.x)*(b.y-a.y);
}

/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(point2D p, point2D q, point2D r) {
  return signed_area2D(p,q,r) == 0;
}

//compute distance between two points in 2D
double distance_formula(point2D a, point2D b) {
  return (sqrt(pow(a.x-b.x,2)+pow(a.y-b.y,2)));
}

/* return 1 if c is  strictly left of ab; 0 if collenar, -1 if right */
int left (point2D a, point2D b, point2D c) {
  if(signed_area2D(a,b,c) > 0)
    return 1;
  else if(signed_area2D(a,b,c) == 0)
    return 0;
  return -1; //else if(signed_area2D(a,b,c) < 0)
}

/* proper intersection test: lines ab and cd */
bool prop_intersect(point2D a, point2D b, point2D c, point2D d) {
  if(collinear(a,b,c) || collinear(a,b,d) || collinear(c,d,a) || collinear(c,d,b))
    return false;

  return
    Xor( left(a,b,c) == 1, left(a,b,d) == 1 ) &&
    Xor( left(c,d,a) == 1, left(c,d,b) == 1 );
}

/* Exlusive or: true iff exactly one arg is true */
bool Xor(bool x, bool y) {
  return !x ^ !y;
}

/* betweenness test, for c beween ab */
bool between(point2D a, point2D b, point2D c) {
  if(!collinear(a,b,c)) return false;

  /* If ab not vertical, check betweeness on x; else on y */
  if(a.x != b.x) {
    return
      ((a.x <= c.x) && (c.x <= b.x)) ||
      ((a.x >= c.x) && (c.x >= b.x));
  }
  else {
    return
      ((a.y <= c.y) && (c.y <= b.y)) ||
      ((a.y >= c.y) && (c.y >= b.y));
  }
}

/* intersection test (proper AND improper): lines ab and cd */
bool intersect(point2D a, point2D b, point2D c, point2D d) {
  if(prop_intersect(a,b,c,d))
    return true;
  else if(between(a,b,c) || between(a,b,d) || between(c,d,a) || between(c,d,b))
    return true;
  else
    return false;
}

point2D segSegInt(point2D a, point2D b, point2D c, point2D d) {
  double s, t;
  double num, denom;
  point2D dummyPoint, intersect;
  dummyPoint.x = -1;
  dummyPoint.y = -1;
  denom =
    a.x*(d.y - c.y) +
    b.x*(c.y - d.y) +
    d.x*(b.y - a.y) +
    c.x*(a.y - b.y);

  if(denom == 0) {
    return dummyPoint;
  }

  num =
    a.x*(d.y - c.y) +
    c.x*(a.y - d.y) +
    d.x*(c.y - a.y);
  s = num/denom;

  num =
    -1*(a.x*(c.y - b.y) +
      b.x*(a.y - c.y) +
      c.x*(b.y - a.y));
  t = num/denom;

  double xCord, yCord;
  xCord = round(a.x + s*(b.x - a.x));
  yCord = round(a.y + s*(b.y - a.y));

  intersect.x = xCord;
  intersect.y = yCord;

  if(0.0 <= s && s <= 1.0 && 0.0 <= t && t <= 1.0) {
    return intersect;
  }
  return dummyPoint;
}

point2D midpoint(point2D a, point2D b) {
  point2D c;
  c.x = (a.x+b.x)/2;
  c.y = (a.y+b.y)/2;
  return c;
}
