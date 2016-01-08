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



#include "XTrig.h"



/*

	//---------- 3d Interpolator -------------------
		int iInterpolatorStepCount = 5;

		static int iInterStep=0;
		if( iInterStep >= iInterpolatorStepCount ){
			iInterStep = 0;
		}else{
			iInterStep++;
		}

		float d = XTrig::distanceBetweenPoints3D(this->zpX, this->zpY, this->zpZ,    this->last_zpX, this->last_zpY, this->last_zpZ );
		float interp_x = iInterStep * (d/(float)iInterpolatorStepCount);
		//printf("step: %i   interp_x: %.3f\n", iInterStep, interp_x );

				float dx = this->last_zpX - this->zpX;
				float dy = this->last_zpY - this->zpY;
				float dz = this->last_zpZ - this->zpZ;

				float np,nh;
				XTrig::computeCourseToLocation3D( dx, dy, dz, &np, &nh );

					float nx,ny,nz;
					XTrig::computeNewPositionOffset3D( nh, np, interp_x, &nx, &nz, &ny );
					p->x = this->last_zpX + nx;
					p->y = this->last_zpY + ny;
					p->z = this->last_zpZ + nz;


		for(int iAgeX=0; iAgeX < iInterStep; iAgeX++ ){
			p->update( fFrp );
		}

*/

std::vector<float>  XTrig::interpolator( float* vec_a, float* vec_b, float steps ){

	//---------- 3d Interpolator -------------------

	float d = XTrig::distanceBetweenPoints3D(vec_a[0], vec_a[1], vec_a[2],    vec_b[0], vec_b[1], vec_b[2] );

	float dx = vec_a[0] - vec_b[0];
	float dy = vec_a[1] - vec_b[1];
	float dz = vec_a[2] - vec_b[2];

	float np,nh;
	XTrig::computeCourseToLocation3D( dx, dy, dz, &np, &nh );

	
	std::vector<float> svFloatStream; //storage for the computed results.

	float nx,ny,nz;
	float interp_x=0.0f;
	
	for( int iInterStep=0; iInterStep < steps; iInterStep++ ){
		interp_x = iInterStep * (d/(float)steps); //this is the distance we need to travel for each step.

		XTrig::computeNewPositionOffset3D( nh, np, interp_x, &nx, &nz, &ny );
		
		svFloatStream.push_back( nx );
		svFloatStream.push_back( ny );
		svFloatStream.push_back( nz );
	}

	return svFloatStream;
}





void XTrig::computeNewPositionOffset(float heading, float speed, float *nx, float *ny){

	heading = (heading-90.0)*RADSCALE;

	*nx = (cos(heading) * speed);
	*ny = (sin(heading) * speed);

	//while it seems the code below is the way to go it is NOT and will result in inverted results and general weirdness.
	/*
	heading = heading * RADSCALE;

	*nx = (sin(heading) * speed);
	*ny = (cos(heading) * speed);
	*/


}


void XTrig::computeNewPositionOffset3D(float heading, float pitch, float speed, float *nx, float *ny, float *nz){

	heading = (heading-90.0)*RADSCALE;
	pitch = (pitch-90.0)*RADSCALE;

	*nx = (cos(heading) * speed * -sin(pitch));
	*ny = (sin(heading) * speed * -sin(pitch));
	*nz = (cos(pitch) * speed);

}


float XTrig::distanceBetweenPoints(float x1, float y1, float x2, float y2){

	//pythag function
	return sqrt( sqr( x2 - x1 ) + sqr( y2 - y1 )  );

}


float XTrig::distanceBetweenPoints3D(float x1, float y1, float z1,    float x2, float y2, float z2 ){

	//     __________________________________
	//d = &#8730; (x2-x1)^2 + (y2-y1)^2 + (z2-z1)^2
	//

	//Our end result
	float result = 0;
	//Take x2-x1, then square it
	double part1 = sqr(x2 - x1);
	//Take y2-y1, then sqaure it
	double part2 = sqr(y2 - y1);
	//Take z2-z1, then square it
	double part3 = sqr(z2 - z1);
	//Add both of the parts together
	double underRadical = part1 + part2 + part3;
	//Get the square root of the parts
	result = sqrt(underRadical);
	//Return our result
	return result;
}


float XTrig::distanceBetweenWorldPoints(float lat1, float lon1, float lat2, float lon2){

    return 2 * asin( sqrt(
                    sqr( (sin( (lat1-lat2)/2 ) ) ) +
                    cos(lat1) * cos(lat2) * sqr( (sin( (lon1-lon2)/2 ))  )
                    )  );

}


float XTrig::computeCourseToLocation(float x, float y){
   	float h = (atan2(y, x) * (180.0f/PI)) + 90.0f;

	if( h < 0.0f ){
		h = 360.0f + h;
	}

    return h;


 //   return (atan2(y, x) * (180.0/PI))+90.0;

}


void XTrig::computeCourseToLocation3D(float x, float y, float z, float *p, float *h){

	*h = XTrig::computeCourseToLocation( x, z );

	float dist = XTrig::distanceBetweenPoints( 0, 0, x, z );

	*p = XTrig::computeCourseToLocation( dist, y ) - 90.0f;


}
