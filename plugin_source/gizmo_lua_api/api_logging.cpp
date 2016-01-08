/*
 *  api_logging.cpp
 *  Gizmo
 *
 *  Created by Ben Russell on 22/03/11.
 *  Copyright 2011 X-Plugins.com. All rights reserved.
 *
 */


/**
logging.debug( "debug message" )
 This function writes a message to the X-Plane Log.txt file.
*/
int gizmo_builtin_logging_debug(lua_State *L){
	
	//ensure we have enough args from lua
	if( lua_gettop(L) != 1 ){ return 0; }
	
	
	if (lua_isstring(L, -1)){
		
		size_t szLen;
		const char *sInput = lua_tolstring( L, -1, &szLen );
		
		XPLMDebugString("Gizmo: logging.debug: (");
		XPLMDebugString(sInput);
		XPLMDebugString(")\n");
		
	}
	
	return 0;
}

