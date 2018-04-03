#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"
#include "../ui/TraceUI.h"

extern TraceUI* traceUI;

class Light
	: public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& P) const = 0;
	virtual double distanceAttenuation( const vec3f& P ) const = 0;
	virtual vec3f getColor( const vec3f& P ) const = 0;
	virtual vec3f getDirection( const vec3f& P ) const = 0;

protected:
	Light( Scene *scene, const vec3f& col )
		: SceneElement( scene ), color( col ) {}

	vec3f 		color;
};

class DirectionalLight
	: public Light
{
public:
	DirectionalLight( Scene *scene, const vec3f& orien, const vec3f& color )
		: Light( scene, color ), orientation( orien ) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f 		orientation;
};

class PointLight
	: public Light
{
public:
	PointLight( Scene *scene, const vec3f& pos, const vec3f& color )
		: Light( scene, color ), position( pos ) {}
	PointLight(Scene *scene, const vec3f& pos, const vec3f& color, double a, double b, double c)
		: Light(scene, color), position(pos), constant_attenuation_coeff(a), linear_attenuation_coeff(b), quadratic_attenuation_coeff(c){
		traceUI->set_m_nConstant_att(a);
		traceUI->set_m_nLinear_att(b);
		traceUI->set_m_nQuadric_att(c);

	}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation( const vec3f& P ) const;
	virtual vec3f getColor( const vec3f& P ) const;
	virtual vec3f getDirection( const vec3f& P ) const;

protected:
	vec3f position;
	double constant_attenuation_coeff, linear_attenuation_coeff, quadratic_attenuation_coeff;
};

#endif // __LIGHT_H__
