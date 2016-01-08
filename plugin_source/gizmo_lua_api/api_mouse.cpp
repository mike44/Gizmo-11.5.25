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
 mouse.setCursor( "CursorDrawingFunctionName" )
 
 Eg: 
	 mouse.setCursor( "drawCustomCursor" )
	 
	 function drawCustomCursor()
		--mouse.x and mouse.y are automatically updated by Gizmo.plugin
		gfx.drawString("mouse cursor", mouse.x, mouse.y)
	 end
 
 A system event exists also: SYS_OnDraw_MouseCursor
 Usage is undefined.
 */
int gizmo_builtin_mouse_setCursor(lua_State *L){

	//ensure we have enough args from lua
	if( lua_gettop(L) != 1 ){ return 0; }

	size_t szLen;
	const char *sInput_CursorDrawingFunction		= lua_tolstring( L, -1, &szLen );
	
		if( strcmp(sInput_CursorDrawingFunction, "") == 0 ){
			//user has sent us a blank string, cancel the drawing callback functions.
			gizmo_UseCustomCursorDrawingCallback = 0;
			gizmo_CustomCursorDrawingCallbackName = "SYS_OnDraw_MouseCursor";
		}else{
			//user has specified a mouse drawing callback.
			gizmo_CustomCursorDrawingCallbackName = sInput_CursorDrawingFunction; //setup the event name global
			gizmo_UseCustomCursorDrawingCallback = 1;	//turn on the code that sends events into lua
		}

	return 0;
}



/**
 mouse.getXY()
 Returns: Mouse X,Y location on the screen. 
	
 Eg: 
	m.x,m.y = mouse.getXY()
 
 */
int gizmo_builtin_mouse_getXY(lua_State *L){

	int x, y;
	XPLMGetMouseLocation( &x, &y );

	lua_pushnumber( L, x );
	lua_pushnumber( L, y );

	return 2;
}


