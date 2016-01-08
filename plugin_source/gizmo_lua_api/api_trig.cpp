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


/**
 trig.courseToLocation( x,y )
 Returns: heading in degrees
 */
int gizmo_builtin_trig_courseToLocation(lua_State *L){ 
	lua_Number lnVX = lua_tonumber( L, -2 );
	lua_Number lnVY = lua_tonumber( L, -1 );

	float c = XTrig::computeCourseToLocation( lnVX, lnVY );
	
	lua_pushnumber( L, c );

	return 1; 
}



/**
 trig.courseToLocation3D( x,y,z )
 Returns: pitch and heading in degrees
 Eg:
 p,h = trig.courseToLocation3D( 1, 2, 3 )
 */
int gizmo_builtin_trig_courseToLocation3D(lua_State *L){
	lua_Number lnVX = lua_tonumber( L, -3 );
	lua_Number lnVY = lua_tonumber( L, -2 );
	lua_Number lnVZ = lua_tonumber( L, -1 );

	float p,h;

	XTrig::computeCourseToLocation3D( lnVX, lnVY, lnVZ, &p, &h );
	
	lua_pushnumber( L, p );
	lua_pushnumber( L, h );

	return 2; 
}



/**
 trig.distanceBetweenPoints( a,b, x,y )
 Returns: distance
 Uses flat plane geometry.
 */
int gizmo_builtin_trig_distanceBetweenPoints(lua_State *L){ 
	lua_Number lnVA = lua_tonumber( L, -4 );
	lua_Number lnVB = lua_tonumber( L, -3 );

	lua_Number lnVX = lua_tonumber( L, -2 );
	lua_Number lnVY = lua_tonumber( L, -1 );


	float c = XTrig::distanceBetweenPoints( lnVA, lnVB,   lnVX, lnVY );
	
	lua_pushnumber( L, c );

	return 1; 
}



/**
 trig.distanceBetweenWorldPoints( a,b, x,y )
 Returns: Returns: distance
 Uses decent spherical geometry translated from web source: ...... (SDK website has it??)
 
 Core Function:
 return 2 * asin( sqrt(
 sqr( (sin( (lat1-lat2)/2 ) ) ) +
 cos(lat1) * cos(lat2) * sqr( (sin( (lon1-lon2)/2 ))  )
 )  );
 
 */
int gizmo_builtin_trig_distanceBetweenWorldPoints(lua_State *L){ 
	lua_Number lnVA = lua_tonumber( L, -4 );
	lua_Number lnVB = lua_tonumber( L, -3 );

	lua_Number lnVX = lua_tonumber( L, -2 );
	lua_Number lnVY = lua_tonumber( L, -1 );


	float c = XTrig::distanceBetweenWorldPoints( lnVA, lnVB,   lnVX, lnVY );
	
	lua_pushnumber( L, c );

	return 1; 
}


/**
 trig.newPositionOffset( heading, speed )
 Returns: x,y
 */
int gizmo_builtin_trig_newPositionOffset(lua_State *L){ 
	lua_Number lnVH = lua_tonumber( L, -2 );
	lua_Number lnVS = lua_tonumber( L, -1 );

	float nx, ny;

	XTrig::computeNewPositionOffset( lnVH, lnVS, &nx, &ny );
	
	lua_pushnumber( L, nx );
	lua_pushnumber( L, ny );

	return 2; 
}


/**
 trig.newPositionOffset3D( heading, pitch, speed )
 Returns: x,y,z
 */
int gizmo_builtin_trig_newPositionOffset3D(lua_State *L){ 
	lua_Number lnVH = lua_tonumber( L, -3 );
	lua_Number lnVP = lua_tonumber( L, -2 );
	lua_Number lnVS = lua_tonumber( L, -1 );

	float nx, ny, nz;

	XTrig::computeNewPositionOffset3D( lnVH, lnVP, lnVS, &nx, &ny, &nz );
	
	lua_pushnumber( L, nx );
	lua_pushnumber( L, ny );
	lua_pushnumber( L, nz );

	return 3; 
}



/**
 trig.interpolator( r,s,y, x,y,z, steps )
 Returns: Stream of x,y,z positions as stream-table.
 Eg:
 xyz_stream = { trig.interpolator( r,s,t, x,y,z, steps ) }
 xyz_stream_len = steps * 3
 
 xyz_stream contains data like this [x,y,z,x,y,z,x,y,z,x,y,z,x,y,z,x,y,z,x,y,z,x,y,z,x,y,z]
 Every 3 variables is one "step" in 3D space.
 */
int gizmo_builtin_trig_interpolator(lua_State *L){

	lua_Number lnVR = lua_tonumber( L, -7 );
	lua_Number lnVS = lua_tonumber( L, -6 );
	lua_Number lnVT = lua_tonumber( L, -5 );

	lua_Number lnVX = lua_tonumber( L, -4 );
	lua_Number lnVY = lua_tonumber( L, -3 );
	lua_Number lnVZ = lua_tonumber( L, -2 );

	lua_Number lnVSteps = lua_tonumber( L, -1 );

	float rst[] = {(float)lnVR, (float)lnVS, (float)lnVT};
	float xyz[] = {(float)lnVX, (float)lnVY, (float)lnVZ};
	
	std::vector<float> svFloatStream = XTrig::interpolator( rst, xyz, (float)lnVSteps );

	for( unsigned int x=0; x < svFloatStream.size(); x++ ){
		lua_pushnumber( L, svFloatStream[x] );
	}
	
	return svFloatStream.size(); 

}

