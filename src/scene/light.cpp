#include <cmath>

#include "light.h"

double DirectionalLight::distanceAttenuation( const vec3f& P ) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation( const vec3f& P ) const
{
    // YOUR CODE HERE:
    // You should implement shadow-handling code here.
	vec3f d = getDirection(P).normalize();
	ray R(P, d);
	isect i;
	vec3f result = getColor(P);
	while (scene->intersect(R, i)) {
		R = ray(R.at(i.t), d);
		result = prod(result, i.getMaterial().kt);
	}
	return result;
}

vec3f DirectionalLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection( const vec3f& P ) const
{
	return -orientation;
}

double PointLight::distanceAttenuation( const vec3f& P ) const
{
	// YOUR CODE HERE

	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0
	return minimum(1.0, 1.0 / (constant_attenuation_coeff + linear_attenuation_coeff * (P - position).length() + quadratic_attenuation_coeff*(P - position).length_squared()));
}

vec3f PointLight::getColor( const vec3f& P ) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection( const vec3f& P ) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	vec3f d = getDirection(P).normalize();
	ray R(P, d);
	isect i;
	vec3f result = getColor(P);
	while (scene->intersect(R, i)) {
		R = ray(R.at(i.t), d);
		result = prod(result, i.getMaterial().kt);
	}
	return result;
}