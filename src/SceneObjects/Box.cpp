#include <cmath>
#include <assert.h>

#include "Box.h"

bool Box::intersectLocal( const ray& r, isect& i ) const
{
	// YOUR CODE HERE:
    // Add box intersection code here.
	// it currently ignores all boxes and just returns false.
	
	double tFar = numeric_limits<double>::infinity();
	double tNear =- numeric_limits<double>::infinity();
	double tMin = -0.5;
	double tMax = 0.5;

	vec3f p = r.getPosition();
	vec3f d = r.getDirection();

	for (int axis = 0; axis < 3; axis++) {
		if (d[axis] == 0) {
			// parallel to i plane
			if (!(p[axis] < tMin || p[axis]>tMax)) {
				// if origin is not between the slabs
				return false;
			}
		}
		else {//not parallel
			double t1 = (tMin - p[axis]) / d[axis];
			double t2 = (tMax - p[axis]) / d[axis];
			if (t1 > t2) {
				std::swap(t1, t2);
			}
			if (t1 > tNear)//largest tNear
				tNear = t1;
			if (t2 < tFar)//smallest tFar
				tFar= t2;
			if (tNear > tFar)//box is missed
				return false;
			if (tFar < 0) //box is behind ray
				return false;
		}
	}
	// intersect! set object material
	i.obj = this;
	//TODO: set normal
	return true;
}
