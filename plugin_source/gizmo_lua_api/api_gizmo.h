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


int gizmo_builtin_gizmo_activate(lua_State *L);
int gizmo_builtin_gizmo_getActivated(lua_State *L);
int gizmo_builtin_gizmo_getDisabledEventNames(lua_State *L);
int gizmo_builtin_gizmo_getPossibleEventNames(lua_State *L);
int gizmo_builtin_gizmo_getVersion(lua_State *L);
int gizmo_builtin_gizmo_getVersionFull(lua_State *L);
int gizmo_builtin_gizmo_reboot(lua_State *L);
int gizmo_builtin_gizmo_resetEventMap(lua_State *L);
int gizmo_builtin_gizmo_resetFirmware(lua_State *L);
int gizmo_builtin_gizmo_setDebugVerbose(lua_State *L);
int gizmo_builtin_gizmo_sleep(lua_State *L);







