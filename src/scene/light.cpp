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

	double constant_attenuation_coeff = traceUI->m_nConstant_att;
	double linear_attenuation_coeff = traceUI->m_nLinear_att;
	double quadratic_attenuation_coeff = traceUI->m_nQuadric_att;

	double coeff = constant_attenuation_coeff + linear_attenuation_coeff * (P - position).length() + quadratic_attenuation_coeff*(position - P).length_squared();
	
	printf("%f, %f, %f, %f,,, ", constant_attenuation_coeff, linear_attenuation_coeff, quadratic_attenuation_coeff, coeff);
	if (coeff <= 0.0) return 1;
		
	return minimum(1.0, 1.0 / coeff);

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
	double distance = (position - P).length();

	vec3f d = getDirection(P);
	ray R(P, d);
	isect i;
	vec3f result = getColor(P);
	while (scene->intersect(R, i)) {
		if ((distance -= i.t) < RAY_EPSILON) return result;
		if (i.getMaterial().kt.iszero()) return vec3f(0, 0, 0);

		R = ray(R.at(i.t), d);
		result = prod(result, i.getMaterial().kt);
	}
	return result;



}