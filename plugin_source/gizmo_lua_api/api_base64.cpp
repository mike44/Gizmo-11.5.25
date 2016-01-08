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
 base64.encode( plain_text_string )
 Returns: base64_encoded_string
 */
int gizmo_builtin_base64_encode(lua_State *L){

	std::string sInput = std::string( lua_tostring( L, -1 ) );
	
		//function sigs in the b64.cpp / b64.h files need to be pre-pended/cleaned.
	
	
	return 0;


} //FIXME: gitbug:64 empty hook function


/**
 base64.decode( base64_encoded_string )
 Returns: plain_text_string
 */
int gizmo_builtin_base64_decode(lua_State *L){return 0;} //FIXME: gitbug:64 empty hook function