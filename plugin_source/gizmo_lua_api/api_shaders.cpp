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


/*
 shaders.api
 Everybody loves Shaders. OpenGL shader access.
 */



/**
 shaders.newShader()
 Returns: shader_id
 */
int gizmo_builtin_shaders_newShader( lua_State *L ){
	XGLShader* tmp = new XGLShader();
	gizmo_pool_XGLShaders.push_back(tmp);
	
	lua_pushinteger( L, gizmo_pool_XGLShaders.size()-1 );
	
	return 1;
};



/**
 shaders.compileShaders( shader_id, vertex_shader_string_blob, fragment_shader_string_blob )
 Returns: boolean for success
 */
int gizmo_builtin_shaders_compileShaders( lua_State *L ){ 

	lua_Integer lnVIndexID = lua_tointeger( L, -3 );

	size_t szLenVert;
	const char *sInputVert = lua_tolstring( L, -2, &szLenVert );

	size_t szLenFrag;
	const char *sInputFrag = lua_tolstring( L, -1, &szLenFrag );

		lua_pushboolean( L, gizmo_pool_XGLShaders[lnVIndexID]->compileShaders( sInputVert, sInputFrag ) );

	return 1;

};




/**
 shaders.useShader( shader_id )
 Activate the specified valid shader_id.
 */
int gizmo_builtin_shaders_useShader( lua_State *L ){ 
	gizmo_pool_XGLShaders[ lua_tointeger( L, -1 ) ]->enable();
	return 0;
};




/**
 shaders.release()
 Return GL to "fixed function hardware" drawing.
 */
int gizmo_builtin_shaders_release( lua_State *L ){ 
	//shortcut
		glUseProgramObjectARB( 0 );
	return 0; 
};






/**
 shaders.setParam( shader_id, "param_name", value )
 Allows passing of dynamic parameters into the shader.
 param_name is a string, value is a floating point number
 */
int gizmo_builtin_shaders_setParam( lua_State *L ){ 
	size_t szLenName;
	gizmo_pool_XGLShaders[ lua_tointeger( L, -3 ) ]->setUniFloat(
																 lua_tolstring( L, -2, &szLenName ),  //name
																 lua_tonumber( L, -1 )	//value
																 );
	return 0; 
};






