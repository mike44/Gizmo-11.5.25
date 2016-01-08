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





int gizmo_builtin_nav_findNavAid(lua_State *L);
int gizmo_builtin_nav_getFirstNavAid(lua_State *L);
int gizmo_builtin_nav_getFirstNavAidOfType(lua_State *L);


int gizmo_builtin_nav_getLastNavAidOfType(lua_State *L);


int gizmo_builtin_nav_getNextNavAid(lua_State *L);
int gizmo_builtin_nav_getNavAidInfo(lua_State *L);

int gizmo_builtin_nav_setSearchGroups(lua_State *L);

int gizmo_builtin_nav_getTypeName(lua_State *L);






// ------ NOT PART OF API -- Support code only. -------


//Returns: Sum total of bitmask flags.
int gizmo_nav_helper_getNavTypeMashup();


//Reference docs: http://www.xsquawkbox.net/xpsdk/mediawiki/XPLMNavigation
/*
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



