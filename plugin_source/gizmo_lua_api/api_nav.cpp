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
navaid_types:
 The following variables are automatically defined by Gizmo.
 
 nav.unknown
 nav.airport
 nav.ndb
 nav.vor
 nav.ils
 nav.localizer
 nav.glideslope
 nav.outermarker
 nav.middlemarker
 nav.innermarker
 nav.fix
 nav.dme
 nav.latlon
 
 */


/**
 nav.findNavAid( "name", "id", lat, lon, frequency )
 Returns: navaid_id
 Combine with nav.setSearchGroups(...)
 This function can be used to search the nav database.
 Needs upgrades to support wildcard name and ids.
 Passing a 0 for lat/lon will set the location to wildcard.
 Passing a 0 for frequency will set the frequency matching to wildcard.
 */
int gizmo_builtin_nav_findNavAid(lua_State *L){

	XPLMDebugString("Gizmo: nav.findNavAid(..)\n");

	int navTypeMashup = gizmo_nav_helper_getNavTypeMashup();
	
	size_t szLen;
	const char *sInputFragment_Name		= lua_tolstring( L, -5, &szLen );
		if( szLen == 0 ){
			XPLMDebugString("\t Name wildcard:  ON\n");
			sInputFragment_Name = NULL;
		}
	const char *sInputFragment_ID		= lua_tolstring( L, -4, &szLen );
		if( szLen == 0 ){
			XPLMDebugString("\t ID wildcard:  ON\n");
			sInputFragment_Name = NULL;
		}


	float fLatVal						= lua_tonumber( L, -3 );
	float fLonVal						= lua_tonumber( L, -2 );
	float* fLatHack;
	float* fLonHack;

	int	iFrequencyVal					= lua_tointeger( L, -1 );
	int* iFrequencyHack;


	if( (fLatVal == 0) || (fLonVal == 0) ){
		fLatHack = NULL;
		fLonHack = NULL;
		XPLMDebugString("\t Fix wildcard:  ON\n");

	}else{
		fLatHack = &fLatVal;
		fLonHack = &fLonVal;
	}

	
	if( iFrequencyVal == 0 ){
		iFrequencyHack = NULL;
		XPLMDebugString("\t Freq wildcard:  ON\n");

	}else{
		iFrequencyHack = &iFrequencyVal;
	}

		XPLMNavRef ret = XPLMFindNavAid(
							   sInputFragment_Name, //const char *         inNameFragment,		// Can be NULL
							   sInputFragment_ID,	//const char *         inIDFragment,		// Can be NULL
							   fLatHack,			//float *              inLat,				// Can be NULL
							   fLonHack,			//float *              inLon,				// Can be NULL
							   iFrequencyHack,		//int *                inFrequency,		// Can be NULL
							   navTypeMashup		//XPLMNavType          inType); 
							   );

		lua_pushinteger( L, ret );

	return 1;
}//nav.findNavAid


/**
 nav.getTypeName( navaid_type )
 Returns: String representation of nav-aid type, see predefined var list at start of this API chapter for basic info.
 */
int gizmo_builtin_nav_getTypeName(lua_State *L){

	lua_Integer lnV_Type = lua_tointeger( L, -1 );
	
	switch( lnV_Type ){
		
		default:
		case xplm_Nav_Unknown:
				lua_pushstring( L, (const char*)"unknown" );
			break;
			
		case xplm_Nav_Airport:
				lua_pushstring( L, (const char*)"airport" );
			break;
			
		case xplm_Nav_NDB:
				lua_pushstring( L, (const char*)"ndb" );
			break;
			
		case xplm_Nav_VOR:
				lua_pushstring( L, (const char*)"vor" );
			break;
			
		case xplm_Nav_ILS:
				lua_pushstring( L, (const char*)"ils" );
			break;
			
		case xplm_Nav_Localizer:
				lua_pushstring( L, (const char*)"localizer" );
			break;
			
		case xplm_Nav_GlideSlope:
				lua_pushstring( L, (const char*)"glideslope" );
			break;
			
		case xplm_Nav_OuterMarker:
				lua_pushstring( L, (const char*)"outermarker" );
			break;
			
		case xplm_Nav_MiddleMarker:
				lua_pushstring( L, (const char*)"middlemarker" );
			break;
			
		case xplm_Nav_InnerMarker:
				lua_pushstring( L, (const char*)"innermarker" );
			break;
			
		case xplm_Nav_Fix:
				lua_pushstring( L, (const char*)"fix" );
			break;
			
		case xplm_Nav_DME:
				lua_pushstring( L, (const char*)"dme" );
			break;
			
		case xplm_Nav_LatLon:
				lua_pushstring( L, (const char*)"latlon" );
			break;
						
	};
	
	return 1;
	
}; //end nav.getTypeName( navaid_type )


/**
 nav.getFirstNavAid()
 Returns: First navaid_id in the database.
 */
int gizmo_builtin_nav_getFirstNavAid(lua_State *L){
	lua_pushinteger( L, XPLMGetFirstNavAid() );
	return 1;
}


/**
 nav.getFirstNavAidOfType( navaid_type )
 Returns: First navaid_id available that matches the specified type.
 */
int gizmo_builtin_nav_getFirstNavAidOfType(lua_State *L){
	lua_pushinteger( L, XPLMFindFirstNavAidOfType( lua_tointeger(L, -1) ) );
	return 1;
}


/**
 nav.getLastNavAidOfType( navaid_type )
 Returns: Last navaid_id available that matches the specified type.
 */
int gizmo_builtin_nav_getLastNavAidOfType(lua_State *L){
	lua_pushinteger( L, XPLMFindLastNavAidOfType( lua_tointeger(L, -1) ) );
	return 1;
}


/**
 nav.getNavAidInfo( navaid_id )
 Returns: Latitude, Longitude, Height, Frequency, Heading, ID, Name, Reg(what is Reg??)
 
 Here is some little known info about "getNavAidInfo"...
 Returning frequency for a type of xplm_Nav_Airport is not supported.
 It is also not supported for a type of xplm_Nav_Fix.
 
 Using all other types will return the frequency.
 This is because this info is in the nav database and not the apt or fix databases
 */
int gizmo_builtin_nav_getNavAidInfo(lua_State *L){

	XPLMNavRef         inRef = (XPLMNavRef)lua_tointeger( L, -1 );
	
	XPLMNavType		   outType=0;
	float              outLatitude=0;
	float              outLongitude=0;
	float              outHeight=0;
	int                outFrequency=0;
	float              outHeading=0;
	
	char             outID[256];
	char             outName[256];
	char             outReg[256];
	
	XPLMGetNavAidInfo(
					  inRef,    
					  &outType,
					  &outLatitude,
					  &outLongitude,
					  &outHeight,
					  &outFrequency,
					  &outHeading,
					  
					  outID,
					  outName,
					  outReg
					  );
	
	//Returns: Latitude, Longitude, Height, Frequency, Heading, ID, Name, Reg(what is Reg??) 
	
		lua_pushinteger( L, outType ); //1
		lua_pushnumber( L, outLatitude ); //2
		lua_pushnumber( L, outLongitude ); //3
		lua_pushnumber( L, outHeight ); //4
		lua_pushinteger( L, outFrequency ); //5
		lua_pushnumber( L, outHeading ); //6
		
		lua_pushstring( L, (const char*)outID ); //7
		lua_pushstring( L, (const char*)outName ); //8
		lua_pushstring( L, (const char*)outReg ); //9
	
	return 9;
}


/**
 nav.getNextNavAid( navaid_type )
 Returns: Next navaid_id available that matches the specified type.
 */
int gizmo_builtin_nav_getNextNavAid(lua_State *L){
	lua_pushinteger( L, XPLMGetNextNavAid( lua_tointeger(L,-1) ) );
	return 1;
}


/**
 nav.setSearchGroups( airports, ndbs, vors, ils, localizers, glideslopes, outermarkers, middle_markers, innermarkers, fixes, dmes, latlons )
 
 Pass a 0 or 1 in for each option value.
 Combine this with nav.findNavAid()
 */
int gizmo_builtin_nav_setSearchGroups(lua_State *L){
	//nav.setSearchGroups( airports, ndbs, vors, ils, localizers, glideslopes, outermarkers, middle_markers, innermarkers, fixes, dmes, latlons )
	
	int argc = lua_gettop(L);
	
	if( argc != 12 ){
		{
			char caDbg[512];
			sprintf(caDbg, "Gizmo: nav.setSearchGroups(...) failed, not enough arguments. Need 12. %i given.\n", argc );
			XPLMDebugString(caDbg);
		}
		return 0; 
	}
	
	gizmo_bitflags_for_nav_search.airport				= lua_tointeger(L,-12);
	gizmo_bitflags_for_nav_search.ndb					= lua_tointeger(L,-11);
	gizmo_bitflags_for_nav_search.vor					= lua_tointeger(L,-10);
	gizmo_bitflags_for_nav_search.ils					= lua_tointeger(L,-9);
	gizmo_bitflags_for_nav_search.localizer				= lua_tointeger(L,-8);
	gizmo_bitflags_for_nav_search.glideslope			= lua_tointeger(L,-7);
	gizmo_bitflags_for_nav_search.outermarker			= lua_tointeger(L,-6);
	gizmo_bitflags_for_nav_search.middlemarker			= lua_tointeger(L,-5);
	gizmo_bitflags_for_nav_search.innermarker			= lua_tointeger(L,-4);
	gizmo_bitflags_for_nav_search.fix					= lua_tointeger(L,-3);
	gizmo_bitflags_for_nav_search.dme					= lua_tointeger(L,-2);
	gizmo_bitflags_for_nav_search.latlon				= lua_tointeger(L,-1);
	
	return 0;
}







/*
 This is an internal helper function to translate to XP-SDK useful values.
 It has no Lua binding.
 */
int gizmo_nav_helper_getNavTypeMashup(){

	int navTypeMashup = 0;

		if( gizmo_bitflags_for_nav_search.airport ){
			navTypeMashup += xplm_Nav_Airport;
		}
		if( gizmo_bitflags_for_nav_search.ndb ){
			navTypeMashup += xplm_Nav_NDB;
		}
		if( gizmo_bitflags_for_nav_search.vor ){
			navTypeMashup += xplm_Nav_VOR;
		}
		if( gizmo_bitflags_for_nav_search.ils ){
			navTypeMashup += xplm_Nav_ILS;
		}
		if( gizmo_bitflags_for_nav_search.localizer ){
			navTypeMashup += xplm_Nav_Localizer;
		}
		if( gizmo_bitflags_for_nav_search.glideslope ){
			navTypeMashup += xplm_Nav_GlideSlope;
		}
		if( gizmo_bitflags_for_nav_search.outermarker ){
			navTypeMashup += xplm_Nav_OuterMarker;
		}
		if( gizmo_bitflags_for_nav_search.middlemarker ){
			navTypeMashup += xplm_Nav_MiddleMarker;
		}
		if( gizmo_bitflags_for_nav_search.innermarker ){
			navTypeMashup += xplm_Nav_InnerMarker;
		}
		if( gizmo_bitflags_for_nav_search.fix ){
			navTypeMashup += xplm_Nav_Fix;
		}
		if( gizmo_bitflags_for_nav_search.dme ){
			navTypeMashup += xplm_Nav_DME;
		}
		if( gizmo_bitflags_for_nav_search.latlon ){
			navTypeMashup += xplm_Nav_LatLon;
		}

	return navTypeMashup;

}; //getNavTypeMashup









