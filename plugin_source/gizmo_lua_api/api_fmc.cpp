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



//typedef int XPLMNavRef;


/**
 fmc.clear()
 Clears all programmed FMC entries.
 */
int gizmo_builtin_fmc_clear(lua_State *L){ //lua ret: nil
	//iterate all entries and clear each one
	int max = XPLMCountFMSEntries();
	for( int x=0; x<max; x++ ){
		XPLMClearFMSEntry( x );
	}
	return 0; 
};


/**
 fmc.clearItem( index_number )
 Clears the FMC step identified by index_number, eg: 2.
 */
int gizmo_builtin_fmc_clearItem(lua_State *L){ //lua ret: adjusted int
	// Adjusted by -1 - Lua is 1 based, C is 0 based.
	XPLMClearFMSEntry( lua_tointeger(L,-1) - 1 ); 
	return 0; 
};


/**
 fmc.getCount()
 Returns: Number of steps programmed into the FMC.
 */
int gizmo_builtin_fmc_getCount(lua_State *L){ //lua ret: int		
	lua_pushinteger( L, XPLMCountFMSEntries() ); 
	return 1; 
};


/**
 fmc.getDestination()
 Returns: Integer telling us which leg of the FMC flight plan the pilot has currently selected as the destination.
 */
int gizmo_builtin_fmc_getDestination(lua_State *L){ //lua ret: adjusted int		
	// Adjusted by +1 - Lua is 1 based, C is 0 based.
	lua_pushinteger( L, XPLMGetDestinationFMSEntry()+1 ); 
	return 1; 
};


/**
 fmc.getDisplayed()
 Returns: Integer telling us which item the user is currently displaying on the FMC.
 */
int gizmo_builtin_fmc_getDisplayed(lua_State *L){ //lua ret: adjusted int
	// Adjusted by +1 - Lua is 1 based, C is 0 based.
	lua_pushinteger( L, XPLMGetDisplayedFMSEntry()+1 ); 
	return 1; 
};


/**
 fmc.getGpsNavAidType()
 Returns: Returns: Integer with a valid navaid_type value from this set: nav.unknown, nav.fix, nav.airport, nav.vor or nav.ndb
 */
int gizmo_builtin_fmc_getGpsNavAidType(lua_State *L){ //lua ret: int (navaid_type)
	//XPLM_API XPLMNavType          XPLMGetGPSDestinationType(void);
	lua_pushinteger( L, XPLMGetGPSDestinationType() );
	return 1; 
};


/**
 fmc.getGpsNavAid()
 Returns: The current navaid_id that the GPS is programmed for.
 */
int gizmo_builtin_fmc_getGpsNavAid(lua_State *L){  //lua ret: int (navaid_id)
	//this function returns a reference to an item in the nav databases.
	//XPLM_API XPLMNavRef           XPLMGetGPSDestination(void);
	lua_pushinteger( L, XPLMGetGPSDestination() );
	return 1; 
};


/**
 fmc.getInfo()
 Returns: navaid_type, NavAid ID String(ICAO?), navaid_id, Latitude, Longitude, Altitude
 Eg:
 navaid_type, NavAid_ID_String, navaid_id, Latitude, Longitude, Altitude = fmc.getInfo( 1 )
 */
int gizmo_builtin_fmc_getInfo(lua_State *L){  //lua ret: multi ret - 6 items
	
	lua_Integer lnV_Id = lua_tointeger( L, -1 );
	
		//storage vars to pull the data out of x-plane
			XPLMNavType tmpNavType = 0;
			
			char tmpNavIdString[8192];
			memset( tmpNavIdString, 0, sizeof(tmpNavIdString) );
			
			XPLMNavRef tmpNavRef = 0;
			float fLat=0.f, fLon=0.f;
			long lAlt=0.f;
			

		XPLMGetFMSEntryInfo(
			   (long)lnV_Id,		//everything except this can be null when passed in if you dont want return data
			   &tmpNavType,			//XPLMNavType *        outType,    
			   tmpNavIdString,		//char *               outID,    
			   &tmpNavRef,			//XPLMNavRef *         outRef,    
			   &lAlt,				//long *               outAltitude,    
			   &fLat,				//float *              outLat,    
			   &fLon				//float *              outLon
		   );    

		
		lua_pushinteger( L, tmpNavType );
		lua_pushstring( L, (const char*)tmpNavIdString );
		lua_pushinteger( L, tmpNavRef );
		
		lua_pushnumber( L, fLat );
		lua_pushnumber( L, fLon );
		lua_pushinteger( L, lAlt );
	
	return 6; 
};



/**
 fmc.setDestination( index_id )
 Controls which leg of the flight plan the FMC will fly towards.
 */
int gizmo_builtin_fmc_setDestination(lua_State *L){		
	//Offset adjusted for Lua
	XPLMSetDestinationFMSEntry(lua_tointeger(L,-1)-1); 
	return 0; 
};



/**
 fmc.setDisplayed( index_id )
 Controls which leg of the flight plan is display on the FMC/FMS.
 */
int gizmo_builtin_fmc_setDisplayed(lua_State *L){		
	//Offset adjusted for Lua
	XPLMSetDisplayedFMSEntry(lua_tointeger(L,-1)-1); 
	return 0; 
};



/**
 fmc.setInfoForItem( index_id, navaid_id, altitude )
 Changes an entry in the FMS. The destination coordinates will be updated to those of the navaid_id passed in and the altitude specified.
 Eg:
 Use this only for airports, fixes, and radio-beacon navaids. 
 Currently the X-Plane FMC/FMS can only support VORs and NDBs. Use the api functions to fly to a lat/lon.
 */
int gizmo_builtin_fmc_setInfoForItem(lua_State *L){ 	
	XPLMSetFMSEntryInfo(
		lua_tointeger( L, -3 ),		//id
		lua_tointeger( L, -2 ),	    //navaid_id
		lua_tointeger( L, -1 )		//Altitude -- SDK param is LONG type
		);
	return 0; 
};


/**
 fmc.setLatLonForItem( index_id, Latitude, Longitude, altitude )
 Changes an entry in the FMS. The destination coordinates will be updated to those of the latitude, longitude and altitude specified.
 */
int gizmo_builtin_fmc_setLatLonForItem(lua_State *L){ 
	XPLMSetFMSEntryLatLon(
		lua_tointeger( L, -4 ),		//id
		lua_tonumber(  L, -3 ),		//lat
		lua_tonumber(  L, -2 ),		//lon
		lua_tointeger( L, -1 )		//Altitude -- SDK param is LONG type
		);
	return 0;
};




