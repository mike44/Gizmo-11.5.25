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
 API: 10.11.22+
 The memcache module provides you string storage over the entire simulator session.
 Think of it as global variables that Gizmo will hold onto for you, you can re-read them after a script reboot.
 Memcache values will be reset when the simulator is restarted.
 */



/**
 memcache.get( "key name" )
 Returns: String containing value stored in key, or nil.
 */
int gizmo_builtin_memcache_get(lua_State *L){
	
	//ensure we have enough args from lua
	if( lua_gettop(L) != 1 ){ return 0; }

		typedef std::map<std::string, std::string> MapType;
		MapType::iterator iter;
		

		size_t szLen;
		const char *sKey		= lua_tolstring( L, -1, &szLen );

		// find will return an iterator to the matching element if it is found
		// or to the end of the map if the key is not found
		iter = gizmo_map_MemCache.find( std::string( sKey ) );
		if (iter != gizmo_map_MemCache.end() ){
			//std::cout << "Value is: " << iter->second << '\n';
			
			lua_pushstring( L, iter->second.c_str() );
			return 1;
			
		//}else{
			//std::cout << "Key is not in my_map" << '\n';
		}


	return 0;
};



/**
 memcache.set( "key name", string_blob )
 Overwrites the value at "key name" with string_blob.
 */
int gizmo_builtin_memcache_set(lua_State *L){

	//ensure we have enough args from lua
	if( lua_gettop(L) != 2 ){ return 0; }

		size_t szLen;
		const char *sKey		= lua_tolstring( L, -2, &szLen );
		const char *sVal		= lua_tolstring( L, -1, &szLen );
	
		gizmo_map_MemCache[ std::string(sKey) ] = std::string(sVal);
	
	return 0;
};


