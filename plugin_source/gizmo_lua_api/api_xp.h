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



int gizmo_builtin_xp_clearReplay(lua_State *L);
int gizmo_builtin_xp_commandBeginByName(lua_State *L);
int gizmo_builtin_xp_commandEndByName(lua_State *L);
int gizmo_builtin_xp_commandOnceByName(lua_State *L);
int gizmo_builtin_xp_getCycleNumber(lua_State *L);
int gizmo_builtin_xp_getDataref(lua_State *L);
int gizmo_builtin_xp_getElapsedTime(lua_State *L);
int gizmo_builtin_xp_getFloat(lua_State *L);
int gizmo_builtin_xp_getFloatV(lua_State *L);
int gizmo_builtin_xp_getInt(lua_State *L);
int gizmo_builtin_xp_getIntV(lua_State *L);
int gizmo_builtin_xp_getLanguage(lua_State *L);
int gizmo_builtin_xp_getOS(lua_State *L);
int gizmo_builtin_xp_getPaused(lua_State *L);
int gizmo_builtin_xp_getPluginsFolder(lua_State *L);
int gizmo_builtin_xp_getVersion(lua_State *L);
int gizmo_builtin_xp_getSDKVersion(lua_State *L);
int gizmo_builtin_xp_getTerrainData(lua_State *L);
int gizmo_builtin_xp_getXPlaneFolder(lua_State *L);

int gizmo_builtin_xp_loadReplay(lua_State *L);
int gizmo_builtin_xp_loadSituation(lua_State *L);

int gizmo_builtin_xp_newCommand(lua_State *L);
int gizmo_builtin_xp_newFloatingPointDataref(lua_State *L);
int gizmo_builtin_xp_newIntegerDataref(lua_State *L);

int gizmo_builtin_xp_reloadScenery(lua_State *L);

int gizmo_builtin_xp_saveReplay(lua_State *L);
int gizmo_builtin_xp_saveSituation(lua_State *L);

int gizmo_builtin_xp_setFloat(lua_State *L);
int gizmo_builtin_xp_setFloatV(lua_State *L);
int gizmo_builtin_xp_setInt(lua_State *L);
int gizmo_builtin_xp_setIntV(lua_State *L);
int gizmo_builtin_xp_setPaused(lua_State *L);




