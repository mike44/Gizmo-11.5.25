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
 http.get( url, callback_function_name )
 Eg:
 function cb_http_get_complete( data, url, size )
 sound.say('dl complete: ' .. url)
 end
 
 url = 'http://example.com/foo.htm'
 sound.say( 'fetch: ' .. url )
 http.get( url, 'cb_http_get_complete' )
 */
int gizmo_builtin_http_get(lua_State *L){

	//ensure we have enough args from lua
	if( lua_gettop(L) != 2 ){ return 0; }


	if (lua_isstring(L, -1)){
		size_t szLen;
		const char *caGetUrl = lua_tolstring( L, -2, &szLen );
		const char *caCbFuncName = lua_tolstring( L, -1, &szLen );

			#if 0
				// *
				XPLMDebugString("Gizmo: http.get(url:'");
					//XPLMDebugString( caGetUrl );
					XPLMDebugString( "<hidden>" );
				XPLMDebugString("', cbfn:'");
					XPLMDebugString( caCbFuncName );
				XPLMDebugString( "');\n" );
				// */
			#endif


			//create a new ix object for use in the threads
			XLuaCurlInterchange* interchangeData = new XLuaCurlInterchange();
				interchangeData->cb_function_name = caCbFuncName; //this is the lua function name to notify when the xfer is done
				interchangeData->url = caGetUrl;
				interchangeData->resetBuffer();
				
				
				interchangeData->useProxy = gizmo_UseCurlProxy;
				interchangeData->proxyString = gizmo_CurlProxyString;
				
				
				int iXPVersion=0;
				int iSDKVersion=0;
				XPLMHostApplicationID xphidFoo;
				XPLMGetVersions( &iXPVersion, &iSDKVersion, &xphidFoo );

				char caCurlUserAgent[512];
				sprintf(caCurlUserAgent, "Gizmo/%s X-Plane/%.2f SDK/%.2f %s", GIZMO_VERSION, (iXPVersion/1000.f), (iSDKVersion/100.f), GIZMO_OS_PLATFORM);
				
				interchangeData->user_agent = std::string(caCurlUserAgent);
				
			//XPLMDebugString("Gizmo: http.get: Created interchange object..\n");



					gizmo_curl_thread_interchange_staging.push_back( interchangeData );
					//XPLMDebugString("Gizmo: http.get: requested added to input queue.\n");


	}//end check for string argument(s)



	return 0;
}



/**
 http.post( url, callback_function_name, post_payload )
 Eg:
 payload = string.format( "status=%s", http.urlEncode("this is a status update") ) 
 
 See HTTP POST format details for more info. 
 It's like a query string but the content isn't auto encoded by a browser.
 */
int gizmo_builtin_http_post(lua_State *L){


	//ensure we have enough args from lua
	if( lua_gettop(L) != 3 ){ return 0; }



	if (lua_isstring(L, -1)){
		size_t szLen;
		const char *caGetUrl = lua_tolstring( L, -3, &szLen );
		//size_t szLen;
		const char *caCbFuncName = lua_tolstring( L, -2, &szLen );

		const char *caPostData = lua_tolstring( L, -1, &szLen );

			
			#if 0
				// *
				XPLMDebugString("Gizmo: http.get(post:'");
					//XPLMDebugString( caGetUrl );
					XPLMDebugString( "<hidden>" );
				XPLMDebugString("', cbfn:'");
					XPLMDebugString( caCbFuncName );
				XPLMDebugString( "');\n" );
				// */
			#endif


			//create a new ix object for use in the threads
			XLuaCurlInterchange* interchangeData = new XLuaCurlInterchange();
				interchangeData->cb_function_name = caCbFuncName; //this is the lua function name to notify when the xfer is done
				interchangeData->url = caGetUrl;
				interchangeData->resetBuffer();
				
				interchangeData->post_data = std::string(caPostData); // <-------post body data

				int iXPVersion=0;
				int iSDKVersion=0;
				XPLMHostApplicationID xphidFoo;
				XPLMGetVersions( &iXPVersion, &iSDKVersion, &xphidFoo );

				char caCurlUserAgent[512];
				sprintf(caCurlUserAgent, "Gizmo/%s X-Plane/%.2f SDK/%.2f %s", GIZMO_VERSION, (iXPVersion/1000.f), (iSDKVersion/100.f), GIZMO_OS_PLATFORM);
				
				interchangeData->user_agent = std::string(caCurlUserAgent);
				
			//XPLMDebugString("Gizmo: http.post: Created interchange object..\n");


					gizmo_curl_thread_interchange_staging.push_back( interchangeData );

			
			//XPLMDebugString("Gizmo: http.post: event completed, returning\n");


		lua_pushboolean( L, 1 );
        return 1;
	}



	lua_pushboolean(L, 0);
	return 1;
}





/**
 http.urlEncode( data, data_length )
 Returns: url-encoded string
 Eg:
 s = "raw string data to send."
 e = http.urlEncode( s, #s )
 http.post(url, cb_function, "data=" .. e)
 */
int gizmo_builtin_http_urlEncode(lua_State *L){

	//ensure we have enough args from lua
	if( lua_gettop(L) != 2 ){ return 0; }


		size_t szLen;
		const char *caTarget	= lua_tolstring( L, -2, &szLen );
		int iSize				= lua_tointeger( L, -1 );

		char* converted = curl_escape( caTarget, iSize );
		lua_pushstring( L, converted );
		curl_free( converted );

		return 1;
};



/**
 http.urlDecode( data, data_length )
 Returns: url-decoded string
 See http.urlEncode for usage example.
 */
int gizmo_builtin_http_urlDecode(lua_State *L){


	//ensure we have enough args from lua
	if( lua_gettop(L) != 2 ){ return 0; }


	size_t szLen;
	const char *caTarget	= lua_tolstring( L, -2, &szLen );
	int iSize				= lua_tointeger( L, -1 );

		char* converted = curl_unescape( caTarget, iSize );
		lua_pushstring( L, converted );
		curl_free( converted );

	return 1;
};



/**
 http.resetProxy()
 Resets internal proxy variables. cURL library will default to using ENV variables if any or no proxy at all.
 */
int gizmo_builtin_http_resetProxy(lua_State *L){

	//This will force cURL back to system defaults, which may be an ENV variable.

	gizmo_UseCurlProxy = 0;
	gizmo_CurlProxyString = "";


	return 0;
}



/**
 http.setProxy( "http://user:pass@proxy.host.lan:8001/" )
 Send a blank string to force use of no proxy for all connections.
 Use http.resetProxy() to reset to default.
 */          
int gizmo_builtin_http_setProxy(lua_State *L){

	//If the user passes a blank string to this function they can force the use of no proxy.
	

	//ensure we have enough args from lua
	if( lua_gettop(L) != 1 ){ return 0; }


	if (lua_isstring(L, -1)){
		size_t szLen;
		const char *caProxyString = lua_tolstring( L, -1, &szLen );
	
		gizmo_UseCurlProxy = 1;
		gizmo_CurlProxyString = caProxyString;
		
	}
	
	return 0;
}




