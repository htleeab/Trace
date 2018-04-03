// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"

// getting depth from UI
#include "ui/TraceUI.h"
extern TraceUI* traceUI;
// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace( Scene *scene, double x, double y )
{
    ray r( vec3f(0,0,0), vec3f(0,0,0) );
    scene->getCamera()->rayThrough( x,y,r );
	return traceRay( scene, r, vec3f(1.0,1.0,1.0), 0 ).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay( Scene *scene, const ray& r, 
	const vec3f& thresh, int depth )
{
	// stop recursive if the depth exceed depth limit set by UI
	if (depth > traceUI->getDepth()) {
		return vec3f(0, 0, 0);
	}

	isect i;

	if( scene->intersect( r, i ) ) {
		// YOUR CODE HERE

		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		vec3f shade;

		const Material& m = i.getMaterial();
		//R = reflect direction = (2*NdL*N)-L
		vec3f L = -r.getDirection();//direction to the light source(the incoming ray)
		vec3f R = 2 * (L.dot(i.N)) * i.N - L;
		ray reflectionRay(r.at(i.t), R);
		vec3f I_r = prod(m.kr, traceRay(scene, reflectionRay, thresh, depth+1));//intensity of reflection

		shade = m.shade(scene, r, i) + I_r;

		//refraction
		
		bool internal_ref = false;

		if (!i.getMaterial().kt.iszero()) {
			double index_p = 1;
			Material m_p;
			//if (!m_stack.empty()) {
			//	m_p = m_stack.top();
			//	index_p = m_p.index;
			//}

			double n = 0.0;
			//going in
			if (i.N*r.getDirection() < -RAY_EPSILON) {
				n = index_p / m.index;
			}
			else { //going out
				n = m.index/ index_p;
				i.N = -i.N;
			}

			

			//calculate ray
			double cosI = -i.N.dot(r.getDirection());
			double sinT2 = n * n * (1.0 - cosI * cosI);
			if (sinT2 > 1.0) internal_ref = true;
			double cosT2 = sqrt(1.0 - sinT2);
			double cosT = sqrt(1 - sinT2);
			vec3f refractionR = n * r.getDirection() + (n * cosI - cosT) * i.N;
			ray refractionRay(r.at(i.t), refractionR);

			if (!internal_ref) {
				m_stack.push(m);
				vec3f I_refraction = prod(m.kt, traceRay(scene, refractionRay, thresh, depth + 1));
				m_stack.pop();

				shade = shade - prod(shade , m.kt);
				shade += shade + I_refraction;
			}

		}
		



		return shade;
	} else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		return vec3f( 0.0, 0.0, 0.0 );
	}
}

RayTracer::RayTracer()
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete [] buffer;
	delete scene;
}

void RayTracer::getBuffer( unsigned char *&buf, int &w, int &h )
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene( char* fn )
{
	try
	{
		scene = readScene( fn );
	}
	catch( ParseError pe )
	{
		fl_alert( "ParseError: %s\n", pe );
		return false;
	}

	if( !scene )
		return false;
	
	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[ bufferSize ];
	
	// separate objects into bounded and unbounded
	scene->initScene();
	
	// Add any specialized scene loading code here
	
	m_bSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup( int w, int h )
{
	if( buffer_width != w || buffer_height != h )
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete [] buffer;
		buffer = new unsigned char[ bufferSize ];
	}
	memset( buffer, 0, w*h*3 );
}

void RayTracer::traceLines( int start, int stop )
{
	vec3f col;
	if( !scene )
		return;

	if( stop > buffer_height )
		stop = buffer_height;

	for( int j = start; j < stop; ++j )
		for( int i = 0; i < buffer_width; ++i )
			tracePixel(i,j);
}

void RayTracer::tracePixel( int i, int j )
{
	vec3f col;

	if( !scene )
		return;

	double x = double(i)/double(buffer_width);
	double y = double(j)/double(buffer_height);

	col = trace( scene,x,y );

	unsigned char *pixel = buffer + ( i + j * buffer_width ) * 3;

	pixel[0] = (int)( 255.0 * col[0]);
	pixel[1] = (int)( 255.0 * col[1]);
	pixel[2] = (int)( 255.0 * col[2]);
}