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
 camera.getAttitude()
 Returns: pitch,roll,heading
 Eg: p,r,h = camera.getAttitude()
 */
int gizmo_builtin_camera_getAttitude(lua_State *L){
	lua_pushnumber( L, gizmo_camera_P );
	lua_pushnumber( L, gizmo_camera_R );
	lua_pushnumber( L, gizmo_camera_H );
	
	return 3;
};




/**
 camera.getControl()
 Ask X-Plane for control of the camera.
 */
int gizmo_builtin_camera_getControl(lua_State *L){
	XPLMControlCamera( 
			xplm_ControlCameraUntilViewChanges,
			gizmo_Camera_ControlFunction,
			NULL
			);
	return 0; 
}


/**
 camera.getControlGreedy()
 Ask X-Plane for control of the camera.
 */
int gizmo_builtin_camera_getControlGreedy(lua_State *L){
	XPLMControlCamera( 
			xplm_ControlCameraForever,
			gizmo_Camera_ControlFunction,
			NULL
			);
	return 0; 
}



/**
 camera.getFOV()
 Returns: Current field of view.
 fov = camera.getFOV()
 */
int gizmo_builtin_camera_getFOV(lua_State *L){

	gizmo_camera_FOV = XPLMGetDataf(gizmo_xpdr_CameraFOV); //import setting to global value to avoid plugin fights.
	
	lua_pushnumber( L, gizmo_camera_FOV );
	
	return 1; 
}


/**
 camera.getPosition()
 Returns: latitude, longitude, altitude
 latitude, longitude, altitude = camera.getPosition()
 */
int gizmo_builtin_camera_getPosition(lua_State *L){
	
	lua_pushnumber( L, gizmo_camera_Lat );
	lua_pushnumber( L, gizmo_camera_Lon );
	lua_pushnumber( L, gizmo_camera_Alt );
	
	return 3; 
}


/**
 camera.getPositionGL()
 Returns: x,y,z
 x,y,z = camera.getPositionGL()
 */
int gizmo_builtin_camera_getPositionGL(lua_State *L){
	
	lua_pushnumber( L, gizmo_camera_Lat );
	lua_pushnumber( L, gizmo_camera_Lon );
	lua_pushnumber( L, gizmo_camera_Alt );
	
	return 3; 
}




/**
 camera.getZoom()
 Returns: zoom
 zoom = camera.getZoom()
 */
int gizmo_builtin_camera_getZoom(lua_State *L){
	
	lua_pushnumber( L, gizmo_camera_Zoom );
	
	return 1; 
}





/**
 camera.lookAt( lat, lot, altitude )
 Rotate the camera in such a way that it is (hopefully) facing the specified location in the world.
 */
int gizmo_builtin_camera_lookAt(lua_State *L){ 

	//intent: camera.lookAtGL( x,y,z )
	//function should:
		//figure out heading, and pitch to target, relative to cameras location.
		//adjust self to face.
	
	double dLat = lua_tonumber( L, -3 );
	double dLon = lua_tonumber( L, -2 );
	double dAlt = lua_tonumber( L, -1 );
	
	double nx,ny,nz;
	
	XPLMWorldToLocal( dLat, dLon, dAlt, &nx, &ny, &nz );

	float x = nx;
	float y = ny;
	float z = nz;
	
	float rel_x = gizmo_camera_X - x;
	float rel_y = gizmo_camera_Y - y;
	float rel_z = gizmo_camera_Z - z;
	
		float p,h;
		XTrig::computeCourseToLocation3D(rel_x, rel_y, rel_z, &p, &h);
	
		gizmo_camera_P = p;
		gizmo_camera_H = h;
 
	return 0; 

}


/**
 camera.lookAtGL( x,y,z )
 Rotate the camera in such a way that it is (hopefully) facing the specified location in OpenGL space.
 */
int gizmo_builtin_camera_lookAtGL(lua_State *L){

	//intent: camera.lookAtGL( x,y,z )
	//function should:
		//figure out heading, and pitch to target, relative to cameras location.
		//adjust self to face.
	
	float x = lua_tonumber( L, -3 );
	float y = lua_tonumber( L, -2 );
	float z = lua_tonumber( L, -1 );
	
	float rel_x = gizmo_camera_X - x;
	float rel_y = gizmo_camera_Y - y;
	float rel_z = gizmo_camera_Z - z;
	
		float p,h;
		XTrig::computeCourseToLocation3D(rel_x, rel_y, rel_z, &p, &h);
	
		gizmo_camera_P = p;
		gizmo_camera_H = h;
 
	return 0; 
}


/**
 camera.releaseControl()
 Release the Camera back into X-Planes hands.
 */
int gizmo_builtin_camera_releaseControl(lua_State *L){

	//this function does not tell us if it's US that has control of the cam, just someone, somewhere?
	if( XPLMIsCameraBeingControlled(NULL) ){
		XPLMDontControlCamera();
	}
	return 0; 
}


/**
 camera.setPosition( lat,lon,alt )
 Set the camera location in world-coordinates.
 */
int gizmo_builtin_camera_setPosition(lua_State *L){ 
	double dLat = lua_tonumber( L, -3 );
	double dLon = lua_tonumber( L, -2 );
	double dAlt = lua_tonumber( L, -1 );
	
	double nx,ny,nz;
	
	XPLMWorldToLocal( dLat, dLon, dAlt, &nx, &ny, &nz );

	gizmo_camera_Lat = dLat;
	gizmo_camera_Lon = dLon;
	gizmo_camera_Alt = dAlt;
	
	gizmo_camera_X = nx;
	gizmo_camera_Y = ny;
	gizmo_camera_Z = nz;

	return 0; 
}


/**
 camera.setPositionGL( x,y,z )
 Set the camera position using OpenGL-coordinates.
 */
int gizmo_builtin_camera_setPositionGL(lua_State *L){ 
	
	gizmo_camera_X = lua_tonumber( L, -3 );
	gizmo_camera_Y = lua_tonumber( L, -2 );
	gizmo_camera_Z = lua_tonumber( L, -1 );

	double ndLat, ndLon, ndAlt;
	XPLMLocalToWorld(  gizmo_camera_X,gizmo_camera_Y,gizmo_camera_Z,  &gizmo_camera_Lat,&gizmo_camera_Lat,&gizmo_camera_Lat  );

	return 0; 
}


/**
 camera.setAttitude( p,r,h )
 Rotate the camera to face a specified orientation.
 */
int gizmo_builtin_camera_setAttitude(lua_State *L){ 
	gizmo_camera_P = lua_tonumber( L, -3 );
	gizmo_camera_R = lua_tonumber( L, -2 );
	gizmo_camera_H = lua_tonumber( L, -1 );
	return 0; 
}


/**
 camera.setFOV( fov )
 Set the field of view for the camera.
 */
int gizmo_builtin_camera_setFOV(lua_State *L){
	
	gizmo_camera_FOV = lua_tonumber( L, -1 );
	
	XPLMSetDataf( gizmo_xpdr_CameraFOV, gizmo_camera_FOV );
	
	return 0; 
}


/**
 camera.setZoom( zoom )
 Set the zoom value for the camera.
 
 API: 11.4+
 
 */
int gizmo_builtin_camera_setZoom(lua_State *L){ 
	gizmo_camera_Zoom = lua_tonumber( L, -1 );
	return 0; 
}





// ------- Support Functions Below -------

bool gizmo_bflag_HoldingCameraControl = false;

int gizmo_Camera_ControlFunction(
                                   XPLMCameraPosition_t * outCameraPosition,    /* Can be NULL */
                                   int                  inIsLosingControl,    
                                   void *               inRefcon)
								   {
	/*
	XPLMCameraPosition_t
	This structure contains a full specification of the camera. X, Y, and Z are the 
	camera's position in OpenGL coordiantes; pitch, roll, and yaw are rotations from
	 a camera facing flat north in degrees. Positive pitch means nose up, positive 
	 roll means roll right, and positive yaw means yaw right, all in degrees. Zoom 
	 is a zoom factor, with 1.0 meaning normal zoom and 2.0 magnifying by 2x (objects appear larger).
	typedef struct {
		 float                     x;
		 float                     y;
		 float                     z;
		 float                     pitch;
		 float                     heading;
		 float                     roll;
		 float                     zoom;
	} XPLMCameraPosition_t;
	*/

	if( inIsLosingControl ){
		//XPLMDebugString("Gizmo: Lost camera control.\n");
		
		gizmo_bflag_HoldingCameraControl = false;
		gizmo_RunLuaFunction( gizmo_luaState, new std::string("OnCameraLost") );
		
		
	}else{
	
		//these globals are all modified using other functions. -_-
		if( ! gizmo_bflag_HoldingCameraControl ){
			gizmo_bflag_HoldingCameraControl = true;
			gizmo_RunLuaFunction( gizmo_luaState, new std::string("OnCameraControl") );	
		}
	
		outCameraPosition->x = gizmo_camera_X;
		outCameraPosition->y = gizmo_camera_Y;
		outCameraPosition->z = gizmo_camera_Z;

		outCameraPosition->pitch = gizmo_camera_P;
		outCameraPosition->heading = gizmo_camera_H;
		outCameraPosition->roll = gizmo_camera_R;

		outCameraPosition->zoom = gizmo_camera_Zoom;
		
	}


	//return 1 to reposition the camera. Return 0 to surrender control of the camera; camera control will be handled by X-Plane on this draw loop.
	return 1;

}


