/*
	Gizmo - A Lua based scripting engine for X-Plane.
    Copyright (C) 2005-2011  Ben Russell, br@x-plugins.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef __X_TRIG__
#define __X_TRIG__
 

#include "X_STL.h"

#include <math.h>
 

#ifndef sqr
#define sqr(x)    ((x)*(x))
#endif
#define PI        3.141592653589793
//radians scale factor (PI/180)
#define RADSCALE  0.0174532925199433


class XTrig{
	public:


		static float computeCourseToLocation(float x, float y);
		static void computeCourseToLocation3D(float x, float y, float z, float *p, float *h);

		static float distanceBetweenPoints(float x1, float y1, float x2, float y2);
		static float distanceBetweenPoints3D(float x1, float y1, float z1,    float x2, float y2, float z2 );
		static float distanceBetweenWorldPoints(float lat1, float lon1, float lat2, float lon2);

		static void computeNewPositionOffset(float heading, float speed, float *nx, float *ny);
		static void computeNewPositionOffset3D(float heading, float pitch, float speed, float *nx, float *ny, float *nz);
		
		
		
		static std::vector<float> interpolator( float* vec_a, float* vec_b, float steps );
	
	
	protected:
	
	private:


};


#endif

