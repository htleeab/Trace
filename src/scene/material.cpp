#include "ray.h"
#include "material.h"
#include "light.h"

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade( Scene *scene, const ray& r, const isect& i ) const
{
	// YOUR CODE HERE

	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
    // You will need to call both distanceAttenuation() and shadowAttenuation()
    // somewhere in your code in order to compute shadows and light falloff.

	// I_phong = k_e + k_a x I_a+sum_j(I_I_j[k_d(N.L_j)+k_j(V*R_j)^N_2]*min(1,1/(	a_0+a_1*d_j+a_2*d_j^2)))
	// reminder: dot(sum). , prod(a1*b1,a2*b2,a3*b3), operater *(vec3f, double) , cross  x
	vec3f P = r.at(i.t);//r.p+r.d*i.t
	vec3f shade = vec3f();
	shade = ke;
	shade += prod(ka, scene->getAmbientLight()); // ambient: k_a x I_a
	//for each light
	for (list<Light*>::const_iterator j = scene->beginLights(); j != scene->endLights(); j++) {
		//diffuse
		vec3f Lj = (*j)->getDirection(P);//direction to the light source
		vec3f diffuse = kd*maximum(i.N.dot(Lj),0);
		//specular
		//R = reflect direction = 2(L.N)N-L (from wiki)
		vec3f R = 2 * (Lj.dot(i.N)) * i.N - Lj;
		vec3f v = -r.getDirection();
		vec3f specular = ks * pow(maximum(R.dot(v), 0.0), shininess*128.0);
		shade += prod( prod( (*j)->getColor(P) , (diffuse + specular) )* (*j)->distanceAttenuation(P) , (*j)->shadowAttenuation(P));

	}
	return shade.clamp();
}
