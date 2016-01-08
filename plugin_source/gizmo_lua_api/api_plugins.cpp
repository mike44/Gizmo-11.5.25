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
 plugins.api 
 X-Plane plugins functions - mainly used for talking to DRE and things like that.
 */







/**
 plugins.findPlugin( "signature" )
 Returns: boolean
 
 Eg:
 
	if( plugins.findPlugin( "custom_systems.xpl" ) )then
		--we could find the plugin and are capable of sending messages to it in a useful manner.
	end
 */
int gizmo_builtin_plugins_findPlugin( lua_State *L ){ 
	
	XPLMDebugString("Gizmo: Looking for a plugin by sig..\n");
	
	if( lua_gettop( L ) == 1 ){
		size_t szLenSig;
		const char *caSig = lua_tolstring( L, -1, &szLenSig );
		
		XPLMPluginID PluginID = XPLMFindPluginBySignature( caSig );
		if (PluginID != XPLM_NO_PLUGIN_ID){
			XPLMDebugString("Gizmo: Looking for a plugin by sig: FOUND\n");

			//found the plugin!
			lua_pushboolean( L, 1 );
		}else{
			XPLMDebugString("Gizmo: Looking for a plugin by sig: NOT FOUND ****\n");

			//could not find the plugin, fail.
			lua_pushboolean( L, 0 );
		}
	}else{
		XPLMDebugString("Gizmo: Looking for a plugin by sig: NOT FOUND - SYNTAX ERROR\n");
		
		lua_pushboolean( L, 0 );	
	}
		
	return 1;
	
};






/**
 plugins.getFolder()
 Returns: path to plugins folder as string.
 
 API: >= 11.5.25
 
 */







/**
 plugins.sendMessage( "signature", code, "message" )
 Returns: true if signature was found and message sent, nil otherwise.
 Eg:
 plugins.sendMessage( "xplanesdk.examples.DataRefEditor", 0x01000000, "myproject/custom/dataref_name" )
 --this would try to register a fake dataref with DRE.
 */
int gizmo_builtin_plugins_sendMessage( lua_State *L ){ 
	
	if( lua_gettop(L) == 3 ){

		size_t szLenSig;
		const char *caSig = lua_tolstring( L, -3, &szLenSig );

		lua_Integer lnMsgCode = lua_tointeger( L, -2 );

		size_t szLenMsg;
		const char *caMsg = lua_tolstring( L, -1, &szLenMsg );

			 XPLMPluginID PluginID = XPLMFindPluginBySignature( caSig );
			 if (PluginID != XPLM_NO_PLUGIN_ID){
				XPLMSendMessageToPlugin(PluginID, (long)lnMsgCode, (void*)caMsg );
				
				lua_pushboolean( L, 1 ); //ret true
				return 1;
				
			 }else{
				 lua_pushboolean( L, 0 ); //ret false
				 return 1;
	 
			 }
		
	}

	return 0;
};



