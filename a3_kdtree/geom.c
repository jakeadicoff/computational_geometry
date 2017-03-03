#include "geom.h"

rect2D buildregion(int top, int bottom, int left, int right) {
  rect2D region;
  region.top = top;
  region.bot = bottom;
  region.left = left;
  region.right = right;

  return region;
}
