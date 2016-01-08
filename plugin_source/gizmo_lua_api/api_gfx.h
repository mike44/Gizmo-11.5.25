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


int gizmo_builtin_gfx_clearTexture(lua_State *L);
int gizmo_builtin_gfx_drawArc(lua_State *L);
int gizmo_builtin_gfx_drawBox(lua_State *L);
int gizmo_builtin_gfx_drawFilledBox(lua_State *L);
int gizmo_builtin_gfx_drawTranslucentDarkBox(lua_State *L);
int gizmo_builtin_gfx_drawCircle(lua_State *L);
int gizmo_builtin_gfx_drawFilledCircle(lua_State *L);
int gizmo_builtin_gfx_drawLine(lua_State *L);
int gizmo_builtin_gfx_drawObject( lua_State *L );
int gizmo_builtin_gfx_drawObjectGL( lua_State *L );
int gizmo_builtin_gfx_drawObjectEx( lua_State *L );



int gizmo_builtin_gfx_drawRadial(lua_State *L);
int gizmo_builtin_gfx_drawString(lua_State *L);


int gizmo_builtin_gfx_enableReflectionDrawingCalls(lua_State *L);

int gizmo_builtin_gfx_disableReflectionDrawingCalls(lua_State *L);

int gizmo_builtin_gfx_getAircraftPositionGL(lua_State *L);


int gizmo_builtin_gfx_getDrawDebugPath(lua_State *L);


int gizmo_builtin_gfx_getFPS(lua_State *L);

int gizmo_builtin_gfx_getFrameSpeed(lua_State *L);

int gizmo_builtin_gfx_getM(lua_State *L);

int gizmo_builtin_gfx_getScreenSize(lua_State *L);



int gizmo_builtin_gfx_getTexture_LiteMap(lua_State *L);
int gizmo_builtin_gfx_getTexture_Paint(lua_State *L);
int gizmo_builtin_gfx_getTexture_UI(lua_State *L);
int gizmo_builtin_gfx_getTexture_GizmoLogo(lua_State *L);
int gizmo_builtin_gfx_getTexture_GizmoUI(lua_State *L);
int gizmo_builtin_gfx_getTexture_XALogo(lua_State *L);
int gizmo_builtin_gfx_getTexture_LuaLogo(lua_State *L);


int gizmo_builtin_gfx_isViewInsideCockpit( lua_State *L );


int gizmo_builtin_gfx_localToWorld( lua_State *L );

int gizmo_builtin_gfx_loadObject( lua_State *L );


int gizmo_builtin_gfx_loadTga( lua_State *L );
int gizmo_builtin_gfx_loadTgaBlob( lua_State *L );

int gizmo_builtin_gfx_matchAircraftOrientation( lua_State *L );

int gizmo_builtin_gfx_measureString(lua_State *L);

int gizmo_builtin_gfx_newFBO(lua_State *L);
int gizmo_builtin_gfx_newTexture(lua_State *L);


int gizmo_builtin_gfx_setColor(lua_State *L);


int gizmo_builtin_gfx_texOff(lua_State *L);
int gizmo_builtin_gfx_texOn(lua_State *L);


int gizmo_builtin_gfx_bakeTerrainMap( lua_State *L );


int gizmo_builtin_gfx_setGraphicsState(lua_State *L);

int gizmo_builtin_gfx_setMaskingTape(lua_State *L);
int gizmo_builtin_gfx_clearMaskingTape(lua_State *L);

int gizmo_builtin_gfx_setTerrainMapMode_Predictive(lua_State *L);
int gizmo_builtin_gfx_setTerrainMapMode_Synthetic(lua_State *L);
int gizmo_builtin_gfx_setTerrainMapMode_Relative(lua_State *L);
int gizmo_builtin_gfx_setTerrainMapMode_Topographic(lua_State *L);

int gizmo_builtin_gfx_setTerrainMapOption_LookAhead(lua_State *L);
int gizmo_builtin_gfx_setTerrainMapOption_NorthUp(lua_State *L);
int gizmo_builtin_gfx_setTerrainMapOption_Range(lua_State *L);
int gizmo_builtin_gfx_setTerrainMapOption_RefreshRate(lua_State *L);
int gizmo_builtin_gfx_setTerrainMapOption_Size(lua_State *L);


int gizmo_builtin_gfx_useFBO(lua_State *L);
int gizmo_builtin_gfx_rebindFBO(lua_State *L);
int gizmo_builtin_gfx_releaseFBO(lua_State *L);
int gizmo_builtin_gfx_useTexture( lua_State *L );
int gizmo_builtin_gfx_worldToLocal(lua_State *L);


