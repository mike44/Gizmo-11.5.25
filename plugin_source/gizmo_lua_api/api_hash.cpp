/*
 *  api_hash.cpp
 *  Gizmo
 *
 *  Created by Ben Russell on 22/03/11.
 *  Copyright 2011 X-Plugins.com. All rights reserved.
 *
 */


/**
hash.md5File( "filename" )
 Returns: string containing md5 hash of file.
 */
int gizmo_builtin_hash_md5File(lua_State *L){
	
	//ensure we have enough args from lua
	if( lua_gettop(L) != 1 ){ return 0; }
	
	
	unsigned char caMd5Hash[128];
	
	if (lua_isstring(L, -1)){
		
		size_t szLen;
		const char *filename = lua_tolstring( L, -1, &szLen );
		
		//XPLMSpeakString( sInput );
		
		gizmo_md5_hash_file( (char*)filename, caMd5Hash );
		
		lua_pushstring(L, (const char*)caMd5Hash);
		
		return 1;
		
	}else{
		return 0;
	}
	
	
}



/**
hash.md5String( "filename" )
 Returns: string containing md5 hash of file.
 */
int gizmo_builtin_hash_md5String(lua_State *L){
	
	//ensure we have enough args from lua
	if( lua_gettop(L) != 1 ){ return 0; }
	
	
	unsigned char caMd5Hash[128];
	
	if (lua_isstring(L, -1)){
		
		size_t szLen;
		const char *sInput = lua_tolstring( L, -1, &szLen );
		
		xplugins_md5( (unsigned char*)sInput, strlen(sInput), caMd5Hash );
		
		lua_pushstring(L, (const char*)caMd5Hash);
		
		return 1;
		
	}else{
		return 0;
	}
	
	
}


