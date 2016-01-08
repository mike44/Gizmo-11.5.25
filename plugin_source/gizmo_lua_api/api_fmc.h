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


// -- Please refer to api_nav.h also for consntants and types. --


int gizmo_builtin_fmc_clear(lua_State *L);
int gizmo_builtin_fmc_clearItem(lua_State *L);
int gizmo_builtin_fmc_getCount(lua_State *L);
int gizmo_builtin_fmc_getDestination(lua_State *L);
int gizmo_builtin_fmc_getDisplayed(lua_State *L);
int gizmo_builtin_fmc_getGpsNavAidType(lua_State *L);
int gizmo_builtin_fmc_getGpsNavAid(lua_State *L);
int gizmo_builtin_fmc_getInfo(lua_State *L);
int gizmo_builtin_fmc_setDestination(lua_State *L);
int gizmo_builtin_fmc_setDisplayed(lua_State *L);
int gizmo_builtin_fmc_setInfoForItem(lua_State *L);
int gizmo_builtin_fmc_setLatLonForItem(lua_State *L);



/*

XPLMNavType

These enumerations define the different types of navaids. They are each defined with a separate bit so that they may be bit-wise added together to form sets of nav-aid types.

NOTE: xplm_Nav_LatLon is a specific lat-lon coordinate entered into the FMS. It will not exist in the database, and cannot be programmed into the FMS. Querying the FMS for navaids will return it. Use XPLMSetFMSEntryLatLon to set a lat/lon waypoint.

xplm_Nav_Unknown	0	
xplm_Nav_Airport	1	
xplm_Nav_NDB	2	
xplm_Nav_VOR	4	
xplm_Nav_ILS	8	
xplm_Nav_Localizer	16	
xplm_Nav_GlideSlope	32	
xplm_Nav_OuterMarker	64	
xplm_Nav_MiddleMarker	128	
xplm_Nav_InnerMarker	256	
xplm_Nav_Fix	512	
xplm_Nav_DME	1024	
xplm_Nav_LatLon	2048	

*/



