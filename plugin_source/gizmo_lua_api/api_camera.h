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




int gizmo_builtin_camera_getAttitude(lua_State *L);

int gizmo_builtin_camera_getControl(lua_State *L);
int gizmo_builtin_camera_getControlGreedy(lua_State *L);


int gizmo_builtin_camera_getFOV(lua_State *L);			//2011-03-16
int gizmo_builtin_camera_getPosition(lua_State *L);		//2011-03-16
int gizmo_builtin_camera_getPositionGL(lua_State *L);	//2011-03-16
int gizmo_builtin_camera_getZoom(lua_State *L);			//2011-03-16

int gizmo_builtin_camera_lookAt(lua_State *L);
int gizmo_builtin_camera_lookAtGL(lua_State *L);
int gizmo_builtin_camera_releaseControl(lua_State *L);


int gizmo_builtin_camera_setAttitude(lua_State *L);
int gizmo_builtin_camera_setFOV(lua_State *L);
int gizmo_builtin_camera_setPosition(lua_State *L);
int gizmo_builtin_camera_setPositionGL(lua_State *L);
int gizmo_builtin_camera_setZoom(lua_State *L);







// -- Support functions and vars below here --

double gizmo_camera_Lat;
double gizmo_camera_Lon;
double gizmo_camera_Alt;

float gizmo_camera_X;
float gizmo_camera_Y;
float gizmo_camera_Z;

float gizmo_camera_P;
float gizmo_camera_R;
float gizmo_camera_H;

float gizmo_camera_FOV;
float gizmo_camera_Zoom;


/*
Actual SDK function that handles interfacing between 
*/
int gizmo_Camera_ControlFunction(
                                   XPLMCameraPosition_t * outCameraPosition,    // Can be NULL
                                   int                  inIsLosingControl,
                                   void *               inRefcon);

