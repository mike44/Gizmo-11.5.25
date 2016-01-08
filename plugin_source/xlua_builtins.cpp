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




// *****************************************************************************************************

//	This is not a normal source file, it is included into XPlaneHooks.cpp.

//	It is not compiled to .o by itself.

//	DO NOT INCLUDE "gizmo_builtins.h" in this file as per normal coding practice.



#pragma GCC visibility push(hidden)

#include "gizmo_lua_api/api_acf.cpp"
#include "gizmo_lua_api/api_base64.cpp"
#include "gizmo_lua_api/api_camera.cpp"
#include "gizmo_lua_api/api_fmc.cpp"
#include "gizmo_lua_api/api_gizmo.cpp"
#include "gizmo_lua_api/api_gfx.cpp"
#include "gizmo_lua_api/api_gui.cpp"
#include "gizmo_lua_api/api_hash.cpp"
#include "gizmo_lua_api/api_http.cpp"
#include "gizmo_lua_api/api_logging.cpp"
#include "gizmo_lua_api/api_matrix.cpp"
#include "gizmo_lua_api/api_memcache.cpp"
#include "gizmo_lua_api/api_menu.cpp"
#include "gizmo_lua_api/api_mouse.cpp"
#include "gizmo_lua_api/api_nav.cpp"
#include "gizmo_lua_api/api_particles.cpp"
#include "gizmo_lua_api/api_physics.cpp"
#include "gizmo_lua_api/api_plugins.cpp"
#include "gizmo_lua_api/api_shaders.cpp"
#include "gizmo_lua_api/api_sound.cpp"
#include "gizmo_lua_api/api_timer.cpp"
#include "gizmo_lua_api/api_traffic.cpp"
#include "gizmo_lua_api/api_trig.cpp"
#include "gizmo_lua_api/api_utils.cpp"
#include "gizmo_lua_api/api_xp.cpp"


#pragma GCC visibility pop



