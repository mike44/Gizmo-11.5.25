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


int gizmo_builtin_acf_getAuthor(lua_State *L);
int gizmo_builtin_acf_getAltAgl(lua_State *L); 
int gizmo_builtin_acf_getAltMsl(lua_State *L); //meters

int gizmo_builtin_acf_getAlpha(lua_State *L); //deg

int gizmo_builtin_acf_getAStabPitch(lua_State *L); //rat

int gizmo_builtin_acf_getAStabRoll(lua_State *L); //rat

int gizmo_builtin_acf_getAStabHeading(lua_State *L); //rat

int gizmo_builtin_acf_getBeta(lua_State *L); //deg


int gizmo_builtin_acf_getFCPitch(lua_State *L); //rat
int gizmo_builtin_acf_getFCRoll(lua_State *L); //rat
int gizmo_builtin_acf_getFCHeading(lua_State *L); //rat

int gizmo_builtin_acf_getGS(lua_State *L); //meters/second

int gizmo_builtin_acf_getHeading(lua_State *L); //deg
int gizmo_builtin_acf_getHeadingTrue(lua_State *L); //deg


int gizmo_builtin_acf_getIAS(lua_State *L);
int gizmo_builtin_acf_getKIAS(lua_State *L);


int gizmo_builtin_acf_getLat(lua_State *L); //deg
int gizmo_builtin_acf_getLon(lua_State *L); //deg

int gizmo_builtin_acf_getPosition(lua_State *L); //returns lat,lon,alt_msl


int gizmo_builtin_acf_getP(lua_State *L);
int gizmo_builtin_acf_getP_dot(lua_State *L);
int gizmo_builtin_acf_getPitch(lua_State *L);
int gizmo_builtin_acf_getQ(lua_State *L);
int gizmo_builtin_acf_getQ_dot(lua_State *L);
int gizmo_builtin_acf_getRoll(lua_State *L);
int gizmo_builtin_acf_getR(lua_State *L);
int gizmo_builtin_acf_getR_dot(lua_State *L);




int gizmo_builtin_acf_getSessionTime(lua_State *L);



int gizmo_builtin_acf_getTAS(lua_State *L); //mtrs/sec
int gizmo_builtin_acf_getTrack(lua_State *L); //track across ground in degrees
int gizmo_builtin_acf_getVS(lua_State *L); //mtrs/sec


int gizmo_builtin_acf_getYokePitch(lua_State *L); //rat
int gizmo_builtin_acf_getYokeRoll(lua_State *L); //rat
int gizmo_builtin_acf_getYokeHeading(lua_State *L); //rat


int gizmo_builtin_acf_getGearDeployState(lua_State *L); //rat[]
int gizmo_builtin_acf_getGearHandleState(lua_State *L); //int


int gizmo_builtin_acf_getAircraftDescription(lua_State *L);
int gizmo_builtin_acf_getAircraftFilename(lua_State *L);
int gizmo_builtin_acf_getAircraftFolder(lua_State *L);

int gizmo_builtin_acf_getRelativeAircraftFolder(lua_State *L);

int gizmo_builtin_acf_reloadVehicle(lua_State *L);
		
int gizmo_builtin_acf_setUsersAircraft(lua_State *L);





