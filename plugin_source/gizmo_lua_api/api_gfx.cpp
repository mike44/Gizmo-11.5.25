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
gfx.getAircraftPositionGL()
Returns: x,y,z,p,r,h
x,y,z,p,r,h = gfx.getAircraftPositionGL()
*/
int gizmo_builtin_gfx_getAircraftPositionGL(lua_State *L){


	float fX = XPLMGetDataf( gizmo_xpdr_Local_X );
	float fY = XPLMGetDataf( gizmo_xpdr_Local_Y );
	float fZ = XPLMGetDataf( gizmo_xpdr_Local_Z );

	float fP = XPLMGetDataf( gizmo_xpdr_Local_P );
	float fH = XPLMGetDataf( gizmo_xpdr_Local_H );
	float fR = XPLMGetDataf( gizmo_xpdr_Local_R );

	

	//xyz position
		lua_pushnumber( L,
						(float)fX
						);

		lua_pushnumber( L,
						(float)fY
						);

		lua_pushnumber( L,
						(float)fZ
						);


	//phr position
		lua_pushnumber( L,
						(float)fP
						);

		lua_pushnumber( L,
						(float)fR
						);

		lua_pushnumber( L,
						(float)fH
						);


	return 6;
}


/**
gfx.getDrawDebugPath()
Returns: true or nil
Can be used to determine whether to draw additional debug data around the aircraft.
*/
int gizmo_builtin_gfx_getDrawDebugPath(lua_State *L){

	lua_pushboolean( L, gizmo_giDebugDrawOn );
	
	return 1;
}


/**
gfx.getFrameSpeed( target_speed_per_second )
Returns: Distance to move this-frame in OpenGL-space to achieve desired speed per second given.
*/
int gizmo_builtin_gfx_getFrameSpeed(lua_State *L){
	/*
	
	//-- codes not real good, buts its what worked. it was cross-tested under varying fps rates. dont understand, not asking. :-/  ..br
	
	2010-11-07 - This is the code that shipped for Icon at OshKosh, changing it breaks all the constants. YUM.
	lua_Number lnV = lua_tonumber( L, -1 ); //argument in Units per second.
	float fps_p =  (1.f/XPLMGetDataf(gizmo_xpdr_FpsMultiplier))/30.f; //How far are we from the ideal FPS?
	
	lua_pushnumber( L, ((float)lnV * XPLMGetDataf(gizmo_xpdr_FpsMultiplier)) * fps_p ); //calculate and push-back the answer in one step.
	
	return 1;
	*/
	
	float fM = XPLMGetDataf(gizmo_xpdr_FpsMultiplier);// * 0.5f;
	
	lua_pushnumber( L, fM * lua_tonumber( L, -1 ) );
	return 1;
	
}


/**
 gfx.getFPS()
 Returns: Frames-per-second value, eg: 50.1
 */
int gizmo_builtin_gfx_getFPS(lua_State *L){
	lua_pushnumber( L, 1.f/XPLMGetDataf(gizmo_xpdr_FpsMultiplier) );
	return 1;
}


/**
 gfx.getM()
 gfx.getFRP()
 Returns: Special multiplier variable^. 
 object_speed_for_this_frame = speed_in_meters * gfx.getM() 
 
 ^ Data comes from: ( sim/operation/misc/frame_rate_period )
 */
int gizmo_builtin_gfx_getM(lua_State *L){
	lua_pushnumber( L, XPLMGetDataf(gizmo_xpdr_FpsMultiplier) );
	return 1;
}



/**
 gfx.getScreenSize()
 Returns: width,height
 local sw,sh = gfx.getScreenSize()
 */
int gizmo_builtin_gfx_getScreenSize(lua_State *L){

	int w, h;
	XPLMGetScreenSize( &w, &h );

	lua_pushnumber( L, w );
	lua_pushnumber( L, h );

	return 2;
}




/**
 gfx.newTexture()
 Returns: texture_id
 Creates a blank texture slot that can be used to load a texture into or create an FBO with.
 */
int gizmo_builtin_gfx_newTexture(lua_State *L){

	if( gizmo_GlobalTexturePoolCursor < gizmo_GlobalTexturePoolSize ){
		
		int iTextureId= gizmo_GlobalTexturePool[ gizmo_GlobalTexturePoolCursor ];
		gizmo_GlobalTexturePoolCursor++;
		
		
		lua_pushinteger( L, iTextureId );

			char caDbg[512];
			sprintf( caDbg, "Gizmo: gfx.newTexture(): returns: %i\n", iTextureId );
			XPLMDebugString( caDbg );

		return 1;
		
	}else{
	
		gizmo_GlobalTexturePoolCursor = 0;
		XPLMDebugString( "Gizmo: Texture pool exhausted. Reset.\n" );
	
		return 0;
		
	}
}




/**
 gfx.getTexture_LiteMap()
 Returns: texture_id for LiteMap texture, not much use with OBJ8 detailed aircraft.
 */
int gizmo_builtin_gfx_getTexture_LiteMap(lua_State *L){
    lua_pushinteger( L, XPLMGetTexture(xplm_Tex_AircraftLiteMap) );
    return 1;
}


/**
 gfx.getTexture_Paint()
 Returns: texture_id for Paint texture, not much use with OBJ8 detailed aircraft.
 */
int gizmo_builtin_gfx_getTexture_Paint(lua_State *L){
    lua_pushinteger( L, XPLMGetTexture(xplm_Tex_AircraftPaint) );
    return 1;
}


/**
 gfx.getTexture_UI()
 Returns: texture_id for X-Plane GUI UI texture. 
 Useful to restore state or draw custom widgets.
 */
int gizmo_builtin_gfx_getTexture_UI(lua_State *L){
    lua_pushinteger( L, XPLMGetTexture(xplm_Tex_GeneralInterface) );
    return 1;
}


/**
 gfx.getTexture_GizmoLogo()
 Returns: texture_id for Gizmo logo texture.
 */
int gizmo_builtin_gfx_getTexture_GizmoLogo(lua_State *L){
    lua_pushinteger( L, gizmo_sys_textures[sys_tex_gizmo_logo] );
    return 1;
}


/**
 gfx.getTexture_GizmoUI()
 Returns: texture_id for Gizmo User Interface elements texture.
 */
int gizmo_builtin_gfx_getTexture_GizmoUI(lua_State *L){
    lua_pushinteger( L, gizmo_sys_textures[sys_tex_gizmo_ui] );
    return 1;
}


/**
 gfx.getTexture_XALogo()
 Returns: texture_id for X-Aviation logo texture.
 */
int gizmo_builtin_gfx_getTexture_XALogo(lua_State *L){
    lua_pushinteger( L, gizmo_sys_textures[sys_tex_xa_logo] );
    return 1;
}


/**
 gfx.getTexture_LuaLogo()
 Returns: texture_id for Lua logo texture.
 */
int gizmo_builtin_gfx_getTexture_LuaLogo(lua_State *L){
    lua_pushinteger( L, gizmo_sys_textures[sys_tex_lua_logo] );
    return 1;
}







/**
 gfx.localToWorld( x,y,z )
 Returns: lat,lon,alt
 Eg:
 lat,lon,alt = gfx.localToWorld( x,y,z )
 */
int gizmo_builtin_gfx_localToWorld(lua_State *L){

		lua_Number lnVX = lua_tonumber( L, -3 );
		lua_Number lnVY = lua_tonumber( L, -2 );
		lua_Number lnVZ = lua_tonumber( L, -1 );

		double dLat, dLon, dAlt;

		XPLMLocalToWorld( (double)lnVX, (double)lnVY, (double)lnVZ,
							&dLat, &dLon, &dAlt );

		lua_pushnumber(L, dLat);
		lua_pushnumber(L, dLon);
		lua_pushnumber(L, dAlt);

	return 3;

}




/**
 gfx.isViewInsideCockpit()
 Returns: boolean
 */
int gizmo_builtin_gfx_isViewInsideCockpit( lua_State *L ){


	//This may be useful for people wishing to draw different geo depending on whether the user is inside or outside the a/c.
//Drawing some kind of flight model visualizations for example may draw unwanted effects into the cockpit view.

	int iDrawMode = XPLMGetDatai( gizmo_xpdr_ViewType );
	//1000 - 2D mode.
	//1026 - 3D mode.
	//other - outside, etc.

	switch( iDrawMode ){
		case 1000: //2D pit
			lua_pushboolean( L, 1 );
			break;

		case 1026: //3D pit
			lua_pushboolean( L, 1 );
			break;

		default: //outside somewhere
			lua_pushboolean( L, 0 );
			break;
	}



	return 1;
}



/**
 gfx.loadObject( [object_id], "Aircraft/General Aviation/Cirrus Jet/objects/cockpit.obj" )
 Returns: object_id or -1 if object could not be loaded by X-Plane.
 */
int gizmo_builtin_gfx_loadObject(lua_State *L){
	
	//TODO: need to magic away the path stuff so they all work.
	
	int lua_arg_count = lua_gettop( L );
	
	if( lua_arg_count == 1 ){
		//one argument, load an ojbect, return its ID.

			size_t szLen;
			const char *sInput = lua_tolstring( L, -1, &szLen );

				{
				char caDbg[1024];
				sprintf(caDbg,"Gizmo: Loading object: %s\n", sInput);
				XPLMDebugString(caDbg);
				}


			XPLMObjectRef tmpObject = XPLMLoadObject( sInput ); //X-Plane is reponsible for destruction, we just use em.
			
				//null check - if we got a null x-plane couldnt load your file.
				if( tmpObject == NULL ){
					XPLMDebugString("Gizmo: Object load failed!\n");
					gizmo_ThrowScriptWarning("Object Load Failed: (" + std::string(sInput) + ")"); //raise event in the script!
					
					lua_pushinteger( L, -1 );
				}else{
					XPLMDebugString("Gizmo: Object Loaded..\n");
					gizmo_XPlaneSceneryObjectHandles.push_back( tmpObject );

					lua_pushinteger( L, gizmo_XPlaneSceneryObjectHandles.size()-1 );

				}

			//we have pushed either -1 for load-failed or the index value of where to find the object.
			return 1;
	

	}else if(lua_arg_count == 2){
		//user is calling us as gfx.loadObject( object_id, object_filename
		
		//get the object ID out of lua
		int iUserObjectId = lua_tointeger( L, -2 );
		//get the filename out of lua
		size_t szLen;
		const char *sInput = lua_tolstring( L, -1, &szLen );

		
		if( iUserObjectId < gizmo_XPlaneSceneryObjectHandles.size() ){
			if( iUserObjectId >= 0 ){
				
				//looks like the user sent us a useful slot handle.
				
				//attempt to load a new object into a new handle
					XPLMObjectRef tmpObject = XPLMLoadObject( sInput ); //X-Plane is reponsible for destruction, we just use em.
					
					//null check - if we got a null x-plane couldnt load your file.
					if( tmpObject == NULL ){
						XPLMDebugString("Gizmo: Object load failed!\n");
						gizmo_ThrowScriptWarning("Object Load Failed: (" + std::string(sInput) + ")"); //raise event in the script!
						
						lua_pushinteger( L, -1 );
					}else{
						
						//unload the existing object so x-plane can free() it
						
						//gizmo_XPlaneSceneryObjectHandles
						
						
						XPLMDebugString("Gizmo: Object Loaded..\n");
						gizmo_XPlaneSceneryObjectHandles[iUserObjectId] = tmpObject;
						
						lua_pushinteger( L, iUserObjectId );
						
					}
				
				
			}
		}
		

	}else{
		gizmo_gui_ScriptError("gfx.loadObect([object_id], object_filename) - wrong number of arguments.");
		
	}//end of argument count check
	
	
}//end gfx.loadObject











/*
 
 //Code snippet from x-alberto: http://forums.x-pilot.com/index.php?topic=1502.msg15474#msg15474
 
 
 x,y,w,h are in panel coordinates
 
 DefineScissor(double x, double y, double w, double h){
 //BEN's advice for SCISSOR
 GLdouble    modelM[16];                        // Our current model view matrix
 GLdouble    projM [16];                        // Our current projection matrix
 GLint      viewport[4];                       // The current viewport
 glGetDoublev (GL_MODELVIEW_MATRIX  ,modelM);
 glGetDoublev (GL_PROJECTION_MATRIX ,projM);
 glGetIntegerv(GL_VIEWPORT          ,viewport);
 
 double x0, y0, z0, x1, y1, z1;
 gluProject(  x,  y, 0.0, modelM, projM, viewport, &x0, &y0, &z0); 
 gluProject(  w,  h, 0.0, modelM, projM, viewport, &x1, &y1, &z1); 
 glScissor(x0, y0, x1, y1);
 }
 
 */


/**
gfx.setMaskingTape( x,y, w,h )
 
 This function is useful for drawing gauges.
 It will setup a masked region for you to draw inside of.
 All drawing outside of the masked region will be ignored.
 
 (Requests an OpenGL clipping box using glScissor(..))
 
 Many thanks to x-alberto@x-pilot for the code submission.
 http://forums.x-pilot.com/index.php?topic=1502.msg15474#msg15474

 
 API: >= 11.5.6
 
 */
int gizmo_builtin_gfx_setMaskingTape(lua_State *L){

	//x,y, w,h are in panel coordinates
	
	glPushAttrib(GL_ALL_ATTRIB_BITS); //this is popped off by gfx.clearMaskingTape()
	
	if( lua_gettop(L) == 4 ){		
		//BEN Supniks's advice for SCISSOR
		GLdouble    modelM[16];                        // Our current model view matrix
		GLdouble    projM [16];                        // Our current projection matrix
		GLint      viewport[4];                       // The current viewport
		glGetDoublev (GL_MODELVIEW_MATRIX  ,modelM);
		glGetDoublev (GL_PROJECTION_MATRIX ,projM);
		glGetIntegerv(GL_VIEWPORT          ,viewport);

		double x,y,w,h;
		x = lua_tonumber(L, -4);
		y = lua_tonumber(L, -3);
		w = lua_tonumber(L, -2);
		h = lua_tonumber(L, -1);

		double x0, y0, z0, x1, y1, z1;
		gluProject(  x,  y, 0.0, modelM, projM, viewport, &x0, &y0, &z0); 
		gluProject(  w,  h, 0.0, modelM, projM, viewport, &x1, &y1, &z1); 
		glEnable( GL_SCISSOR_TEST );
		glScissor(x0, y0, x1, y1);
	
	}else{
		char tmp[2048];
		sprintf( tmp, "gfx.setMaskingTape( x,y, w,h ) expected 4 arguments, got: %i", lua_gettop(L) );
		
		gizmo_gui_ScriptError(tmp);
		
	}
	
	return 0;
	
}



/**
gfx.clearMaskingTape()

 This function is useful for drawing gauges.
 See gfx.setMaskingTape(..) for more info.
 */
int gizmo_builtin_gfx_clearMaskingTape(lua_State *L){
	
	glPopAttrib();
	
	return 0;
		
}









/**
 gfx.setColor( r,g,b, a )
 All arguments are 0-1
 Set the color to red, 75% opacity.
 gfx.setColor( 1,0,0, 0.75 )
 */
int gizmo_builtin_gfx_setColor(lua_State *L){


		//size_t szLen;
		lua_Number lnVR = lua_tonumber( L, -4 );
		lua_Number lnVG = lua_tonumber( L, -3 );
		lua_Number lnVB = lua_tonumber( L, -2 );
		lua_Number lnVA = lua_tonumber( L, -1 );

        gizmo_faDrawStringPenColor[0] = (float)lnVR;
        gizmo_faDrawStringPenColor[1] = (float)lnVG;
        gizmo_faDrawStringPenColor[2] = (float)lnVB;

        glColor4f( (float)lnVR, (float)lnVG, (float)lnVB, (float)lnVA );


	return 0;
}



/**
 gfx.setState( fog, numTexUnits, lighting, alphaTest, alphaBlend, depthTest, depthWrite )
 Set graphics state using XPLMSetGraphicsState(..) Most flags are 0 or 1.
 numTexUnits accepts > 1
 */
int gizmo_builtin_gfx_setGraphicsState(lua_State *L){

	lua_Integer lnV_Fog			= lua_tointeger( L, -7 );
	lua_Integer lnV_TexUnits	= lua_tointeger( L, -6 );
	lua_Integer lnV_Lighting	= lua_tointeger( L, -5 );

	lua_Integer lnV_AlphaTest	= lua_tointeger( L, -4 );
	lua_Integer lnV_AlphaBlend	= lua_tointeger( L, -3 );

	lua_Integer lnV_DepthTest	= lua_tointeger( L, -2 );
	lua_Integer lnV_DepthWrite	= lua_tointeger( L, -1 );

		XPLMSetGraphicsState(
							 (int)lnV_Fog,//1, //fog
							 (int)lnV_TexUnits,//10, //num tex units
							 (int)lnV_Lighting,//1, //lighting
							 (int)lnV_AlphaTest,//1, //alpha test
							 (int)lnV_AlphaBlend,//1, //alpha blend
							 (int)lnV_DepthTest,//1, //depth test
							 (int)lnV_DepthWrite//1  //depth write
							 ); //very important!

	return 0;
}



/**
 gfx.texOff()
 Turn OpenGL texturing off.
 */
int gizmo_builtin_gfx_texOff(lua_State *L){

		XPLMSetGraphicsState(
							 0, //(int)lnV_Fog,//1, //fog
							 0, //num tex units
							 0, //(int)lnV_Lighting,//1, //lighting
							 1, //(int)lnV_AlphaTest,//1, //alpha test
							 1, //(int)lnV_AlphaBlend,//1, //alpha blend
							 1, //(int)lnV_DepthTest,//1, //depth test
							 1 //(int)lnV_DepthWrite//1  //depth write
							 ); //very important!

	return 0;
}



/**
 gfx.texOn()
 Turn OpenGL texturing on.
 */
int gizmo_builtin_gfx_texOn(lua_State *L){

		XPLMSetGraphicsState(
							 0, //(int)lnV_Fog,//1, //fog
							 1, //num tex units
							 0, //(int)lnV_Lighting,//1, //lighting
							 1, //(int)lnV_AlphaTest,//1, //alpha test
							 1, //(int)lnV_AlphaBlend,//1, //alpha blend
							 1, //(int)lnV_DepthTest,//1, //depth test
							 1 //(int)lnV_DepthWrite//1  //depth write
							 ); //very important!

	return 0;
}



/**
 gfx.useFBO( fbo_id )
 Makes an FBO object active and redirects all OpenGL drawing into that FBO. 
 All drawing will be saved into the texture nominated during gfx.newFBO(..) 
 You MUST call gfx.releaseFBO() when you are finished drawing.
 */
int gizmo_builtin_gfx_useFBO(lua_State *L){

	lua_Integer lnX = lua_tointeger( L, -1 );
	XLuaFboWrapper* h_fbo = gizmo_sv_FboWrappers[lnX];
	h_fbo->use();

	return 0;
}



/**
gfx.clearTexture( texture_id, widht, height, bpp )
 
 This function will erase an OpenGL texture slot.
 
 width and height should be power of 2 numbers for maximum compatability.
 
 bpp may be either 32 or 24.
	32 will upload a black texture with a clear alpha channel to the texture.
	24 will upload a black texture with no alpha channel data to the texture.
 
 */
int gizmo_builtin_gfx_clearTexture(lua_State *L){

	//feature complete! sweet.
	if( lua_gettop( L ) != 4 ){
		gizmo_gui_ScriptError("gfx.Texture() - wrong number of arguments. See docs.");
		return 0;
	}

	
	lua_Integer lnX = lua_tointeger( L, -4 ); //textud id
	lua_Integer lnW = lua_tointeger( L, -3 );
	lua_Integer lnH = lua_tointeger( L, -2 );
	lua_Integer lnBPP = lua_tointeger( L, -1 );

		if( lnBPP == 32 ){
			glBindTexture(GL_TEXTURE_2D, lnX);
			void* blob = calloc(1024 * 1024 * 4, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  lnW, lnH, 0, GL_RGBA, GL_UNSIGNED_BYTE, blob);
			free( blob );
		}else{
			glBindTexture(GL_TEXTURE_2D, lnX);
			void* blob = calloc(1024 * 1024 * 3, 1);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8,  lnW, lnH, 0, GL_RGB, GL_UNSIGNED_BYTE, blob);
			free( blob );		
		}
			
	return 0;
}



/**
 gfx.rebindFBO( fbo_id..... )
 BROKEN: gitbug:12
 */
int gizmo_builtin_gfx_rebindFBO(lua_State *L){

	//feature complete! sweet.

	lua_Integer lnX		= lua_tointeger( L, -4 );
	lua_Integer lnTex	= lua_tointeger( L, -3 );
	lua_Integer lnW		= lua_tointeger( L, -2 );
	lua_Integer lnH		= lua_tointeger( L, -1 );
	
	
	XLuaFboWrapper* h_fbo = gizmo_sv_FboWrappers[lnX];
	h_fbo->rebind( lnTex, lnW, lnH);

	return 0;
}


/**
 gfx.releaseFBO( fbo_id )
 Release active FBO and return all drawing targets to normal.
 You MUST pass a valid fbo_id handle. 
 It doesn't really matter which one. 
 If you don't the OpenGL state will be scrambled.
 */
int gizmo_builtin_gfx_releaseFBO(lua_State *L){

	//feature complete! sweet.

	lua_Integer lnX = lua_tointeger( L, -1 );
	XLuaFboWrapper* h_fbo = gizmo_sv_FboWrappers[lnX];
	h_fbo->release();

	return 0;
}





/**
 gfx.useTexture( texture_id )
 Sets your nominated texture as "active" for OpenGL drawing operations.
 */
int gizmo_builtin_gfx_useTexture(lua_State *L){

		lua_Integer lnVIndexID = lua_tointeger( L, -1 );
		//XPLMBindTexture2d( gizmo_TextureSlots[(int)lnVIndexID], GL_TEXTURE_2D);

		XPLMBindTexture2d( (int)lnVIndexID, GL_TEXTURE_2D);

		//printf("Gizmo: bindTexture( %i );\n", gizmo_TextureSlots[(int)lnVIndexID]);

	return 0;
}



/**
 gfx.loadTga( [texture_id], "/full/path/to/texture.tga" )
 Returns: texture_id
 Load a TGA file into the specified texture slot. Acceptable formats: 24 or 32 bit. Do not use RLE encoding.
 */
int gizmo_builtin_gfx_loadTga(lua_State *L){

	int iArgCount = lua_gettop( L );
	
	int iTextureId = -1;
	
	
	switch( iArgCount ){
		case 1:{
			if (lua_isstring(L, -1)){

				iTextureId = gizmo_GlobalTexturePool[ gizmo_GlobalTexturePoolCursor ];
				gizmo_GlobalTexturePoolCursor++;
				
				
				size_t szLen;
				const char *sInput = lua_tolstring( L, -1, &szLen );
				{
					char dbg[1024];
					sprintf(dbg,"Gizmo: gfx.loadTga(\"%s\") returns: %i \n", (char*)sInput, iTextureId );
					XPLMDebugString(dbg);
				}
					gizmo_LoadTGA_File( iTextureId, (char*)sInput );

			}
		}
		break;

		case 2:{
			if (lua_isstring(L, -1)){
				
				iTextureId = lua_tointeger( L, -2 );
				size_t szLen;
				const char *sInput = lua_tolstring( L, -1, &szLen );
				{
					char dbg[1024];
					sprintf(dbg,"Gizmo: gfx.loadTga( %i, \"%s\" ) returns: %i \n", iTextureId, (char*)sInput, iTextureId );
					XPLMDebugString(dbg);
				}
				gizmo_LoadTGA_File( iTextureId, (char*)sInput );
				
			}
		}
		break;
			
		//TODO: default branch to spit out debug message for gfx.loadTga()
			
	}//end switch( iArgCount )

	
	lua_pushnumber( L, iTextureId );
	return 1;

}



/**
 gfx.loadTgaBlob( texture_id, blob, blob_size )
 Load a TGA blob into the specified texture slot. Acceptable formats: 24 or 32 bit. Do not use RLE encoding.
 This can be used to download a texture off the disk or bake one in RAM, etc.
 */
int gizmo_builtin_gfx_loadTgaBlob(lua_State *L){


	if (lua_isstring(L, -1)){

		lua_Integer lnVIndexID = lua_tointeger( L, -3 );
		size_t szLen;
		const char *sInput = lua_tolstring( L, -2, &szLen );
		lua_Integer lnVBlobSize = lua_tointeger( L, -1 );

			gizmo_LoadTGA_DecodeHeaders( (int)lnVIndexID, (unsigned char*)sInput, (int)lnVBlobSize );

			//printf("Gizmo: gfx.loadTgaBlob( %i, ..., %i );\n", (int)lnVIndexID, (int)lnVBlobSize);

	}

	return 0;

}



int gizmo_builtin_gfx_loadSTga(lua_State *L){


	if (lua_isstring(L, -1)){

		lua_Integer lnVIndexID = lua_tointeger( L, -2 );
		size_t szLen;
		const char *sInput = lua_tolstring( L, -1, &szLen );

			//XPLMSpeakString( sInput );

			gizmo_LoadTGA_File( (int)lnVIndexID, (char*)sInput );

			//printf("Gizmo: loadSTga( %i, \"%s\" );\n", (int)lnVIndexID, (char*)sInput);


	}

	return 0;

}



/**
 gfx.drawString( string, [x,y] )
 Draws a string using the default X-Plane font.
 No other font is available for this function.
 x,y parameters are optional, 0,0 is the default if not specified.
 */
int gizmo_builtin_gfx_drawString(lua_State *L){

	int arg_count = lua_gettop(L);
	switch( arg_count ){
		case 3:
			{
				size_t szLen;
				const char *sInput = lua_tolstring( L, -3, &szLen );
				if( szLen > 0 ){
					lua_Integer lnVX = lua_tointeger( L, -2 );
					lua_Integer lnVY = lua_tointeger( L, -1 );
					XPLMDrawString( gizmo_faDrawStringPenColor, (int)lnVX, (int)lnVY, (char*)sInput, NULL, xplmFont_Basic );
				}
			}
			break;

		case 1:
			{
				size_t szLen;
				const char *sInput = lua_tolstring( L, -1, &szLen );
				if( szLen > 0 ){
					XPLMDrawString(	gizmo_faDrawStringPenColor, 0, 0, (char*)sInput, NULL, xplmFont_Basic );
				}
			}
			break;
	}

	return 0;

}





/**
 gfx.disableReflectionDrawingCalls()
 Unregister your scripts from X-Planes reflection-drawing calls.
 */
int gizmo_builtin_gfx_disableReflectionDrawingCalls(lua_State *L){

	XPLMEnableFeature("XPLM_WANTS_REFLECTIONS",0);
	return 0;

}


/**
 gfx.enableReflectionDrawingCalls()
 Register your scripts with X-Planes reflection-drawing calls.
 This results in your drawing callbacks being called many times.
 */
int gizmo_builtin_gfx_enableReflectionDrawingCalls(lua_State *L){

	XPLMEnableFeature("XPLM_WANTS_REFLECTIONS",1);	
	return 0;

}




/**
 gfx.newFBO( texture_id, width, height )
 Returns: fbo_id
 Creates a new Frame-Buffer-Object that can be used to "render to texture".
 See also gfx.useFBO(..) and gfx.releaseFBO(..)
 */
int gizmo_builtin_gfx_newFBO(lua_State *L){

	if( lua_gettop(L) < 3 ){
		XPLMDebugString("Gizmo: gfx.newFBO(..) - Syntax error: not enough arguments.\n");
		return 0;
	}

	//args? width, height, texId ?

	lua_Integer lnX = lua_tointeger( L, -3 );
	lua_Integer lnW = lua_tointeger( L, -2 );
	lua_Integer lnH = lua_tointeger( L, -1 );

	int tex_id = lnX;

		XLuaFboWrapper* h_fbo = new XLuaFboWrapper( tex_id, lnW, lnH );
		
		int iNewFboId = gizmo_sv_FboWrappers.size(); //will incrase by one when we push onto the vector.
		gizmo_sv_FboWrappers.push_back( h_fbo );
		

	lua_pushinteger( L, iNewFboId );

	return 1;

}



/**
 gfx.matchAircraftOrientation()
 Performs a GL translate and rotate so that your drawing matches the aircraft exactly. 
 From here you can easily translate to the nose or a wing tip.
 Note: You MUST wrap this function in calls to gl.PushMatrix() before you call and gl.PopMatrix() after you call. 
 If you do not you will corrupt the drawing stack and the result is undefined. Probably junk on screen.
 */
int gizmo_builtin_gfx_matchAircraftOrientation( lua_State *L ){

	//no args expected

	float fX = XPLMGetDataf( gizmo_xpdr_Local_X );
	float fY = XPLMGetDataf( gizmo_xpdr_Local_Y );
	float fZ = XPLMGetDataf( gizmo_xpdr_Local_Z );

	float fP = XPLMGetDataf( gizmo_xpdr_Local_P );
	float fH = XPLMGetDataf( gizmo_xpdr_Local_H );
	float fR = XPLMGetDataf( gizmo_xpdr_Local_R );

		//match the position and location of the aircraft
		glTranslatef(fX, fY, fZ);
			glRotatef( fH,	0.0f, -1.0f, 0.0f );  //heading
			glRotatef( fP,	1.0f, 0.0f, 0.0f); //pitch
			glRotatef( fR,	0.0f, 0.0f, -1.0f);	//roll


	return 0;
}



/**
 gfx.measureString( "string" )
 Returns: horizontal length of string in decimal pixels
 */
/**
 gfx.getStringWidth( "string" )
 Returns: horizontal length of string in decimal pixels
 */
int gizmo_builtin_gfx_measureString(lua_State *L){

	float r = 0.f;

	if( lua_gettop(L) < 1 ){
		XPLMDebugString("Gizmo: gfx.measureString(..) - Syntax error: not enough arguments.\n");
		return 0;
	}

	if (lua_isstring(L, -1)){

		size_t szLen;
		const char *sInput = lua_tolstring( L, -1, &szLen );

			r = XPLMMeasureString( xplmFont_Basic, (char*)sInput, strlen((char*)sInput)  );

	}

	lua_pushnumber(L, r);

	return 1;

}




/**
gfx.drawArc( radius, segments, begin_degrees, end_degrees )
 
 Draws an arc line, use gl.api to setup line styling parameters.
 
 radius in pixels
 segments controls the subdivision level for the arc drawing, higher = smoother.
 begin and end_degrees start at 12'oclock and wind clockwise.
 
 */
int gizmo_builtin_gfx_drawArc(lua_State *L){

	if( lua_gettop( L ) != 4 ){
		gizmo_gui_ScriptError("gfx.drawArc() - wrong number of arguments. See docs.");
		return 0;
	}
	
	
		lua_Number lnVradius		= lua_tonumber( L, -4 );
		lua_Number lnVsegments		= lua_tonumber( L, -3 );
		lua_Number lnVbegin			= lua_tonumber( L, -2 );
		lua_Number lnVend			= lua_tonumber( L, -1 );

		
		if( lnVend == lnVbegin ){ return 0; } //fail quietly instead of loop lock..

		
			float fOffX, fOffY;

			float fStepSize = (lnVend - lnVbegin) / lnVsegments;
			float fStart = lnVbegin;// - fStepSize;

				glBegin( GL_LINE_STRIP );
				
					for( float hx=fStart; hx<=lnVend; hx+=fStepSize ){
						XTrig::computeNewPositionOffset( hx, lnVradius, &fOffX, &fOffY );
						glVertex2f( fOffX, -fOffY );
					}

				glEnd();
	return 0;

}





/**
gfx.drawBox( left,bottom,width,height )

 Draws a box on the screen, does not fill it.
 Use gl.api to modify line styling params.
 
 */
int gizmo_builtin_gfx_drawBox(lua_State *L){

	if( lua_gettop( L ) != 4 ){
		gizmo_gui_ScriptError("gfx.Texture() - wrong number of arguments. See docs.");
		return 0;
	}

		lua_Number lnVx = lua_tonumber( L, -4 ); //l
		lua_Number lnVy = lua_tonumber( L, -3 ); //b
		lua_Number lnVw = lua_tonumber( L, -2 ); //w
		lua_Number lnVh = lua_tonumber( L, -1 ); //h

			glBegin( GL_LINE_LOOP );
				//clockwise winding..
				glVertex2f( lnVx, lnVy ); //l,b
					glVertex2f( lnVx, lnVy+lnVh ); //l,t
					glVertex2f( lnVx+lnVw, lnVy+lnVh ); //r,t
				glVertex2f( lnVx+lnVw, lnVy ); //r,b
			glEnd();

	return 0;

}


/**
gfx.drawTranslucentDarkBox( left,bottom,width,height )
 
 Draws a transparent dark box using the built in Plugin SDK function: XPLMDrawTranslucentDarkBox(...)
 
 */
int gizmo_builtin_gfx_drawTranslucentDarkBox(lua_State *L){

	if( lua_gettop( L ) != 4 ){
		gizmo_gui_ScriptError("gfx.drawTranslucentDarkBox() - wrong number of arguments. See docs.");
		return 0;
	}
	
	lua_Integer lnVX = lua_tointeger( L, -4 );
		lua_Integer lnVY = lua_tointeger( L, -3 );
		lua_Integer lnVW = lua_tointeger( L, -2 );
		lua_Integer lnVH = lua_tointeger( L, -1 );

			XPLMDrawTranslucentDarkBox(		(int)lnVX, //left
											(int)lnVY + lnVH, //bottom
											((int)lnVX + (int)lnVW), //width
											((int)lnVY - (int)lnVH)  //height
											);

	return 0;

}




/**
gfx.drawCircle( radius,segments )
 
 Draws a circular line, use gl.api to setup line styling parameters.
 
 radius in pixels
 segments controls the subdivision level for the arc drawing, higher = smoother.
 
 */
int gizmo_builtin_gfx_drawCircle(lua_State *L){
	
	if( lua_gettop( L ) != 2 ){
		gizmo_gui_ScriptError("gfx.drawCircle() - wrong number of arguments. See docs.");
		return 0;
	}
	
		
	lua_Number lnVradius = lua_tonumber( L, -2 );
		lua_Number lnVsegments = lua_tonumber( L, -1 );

			float fOffX, fOffY;

			float fStepSize = 360.f / lnVsegments;
			float fStart = -fStepSize;

				glBegin( GL_LINE_LOOP );

					for( float hx=fStart; hx<360.f; hx+=fStepSize ){ //36 seg
						XTrig::computeNewPositionOffset( hx, lnVradius, &fOffX, &fOffY );
						glVertex2f( fOffX, fOffY );
					}

				glEnd();

	return 0;

}


/**
gfx.drawFilledCircle( radius,segments )

 Draws a filled circle, use gl.api to setup styling parameters.
 
 radius in pixels
 segments controls the subdivision level for the arc drawing, higher = smoother.
 
 */
int gizmo_builtin_gfx_drawFilledCircle(lua_State *L){
	
	if( lua_gettop( L ) != 2 ){
		gizmo_gui_ScriptError("gfx.drawFilledCircle() - wrong number of arguments. See docs.");
		return 0;
	}
	

		lua_Number lnVradius = lua_tonumber( L, -2 );
		lua_Number lnVsegments = lua_tonumber( L, -1 );

			float fOffX, fOffY;

			float fStepSize = 360.f / lnVsegments;
			float fStart = -fStepSize;

				glBegin( GL_POLYGON );

					for( float hx=360.f; hx>fStart; hx-=fStepSize ){
					//for( float hx=fStart; hx<360.f; hx+=fStepSize ){
						XTrig::computeNewPositionOffset( hx, lnVradius, &fOffX, &fOffY );
						glVertex2f( fOffX, fOffY );
						
						#if 0
						{
							//Code to diagnose winding order.
							char caDbg[1024];
							sprintf(caDbg, "Gizmo: Filled circle: hdg: %0.3f  fix: %0.3f / %0.3f\n", hx, fOffX, fOffY);
							XPLMDebugString( caDbg );
						}
						#endif
						
					}

				glEnd();

	return 0;

}


/**
gfx.drawLine( a,b, x,y )
 
 Draws a line, use gl.api to setup line styling parameters.
 
	--Draw a line from the bottom left corner of the screen/texture
	--To approx mid-point. Assuming screen/texture is 1024^2
 
	gfx.drawLine( 0,0, 512,512 );

 */
int gizmo_builtin_gfx_drawLine(lua_State *L){

	if( lua_gettop( L ) != 4 ){
		gizmo_gui_ScriptError("gfx.drawLine() - wrong number of arguments. See docs.");
		return 0;
	}
	
	
	lua_Number a = lua_tonumber( L, -4 );
	lua_Number b = lua_tonumber( L, -3 );
	
	lua_Number x = lua_tonumber( L, -2 );
	lua_Number y = lua_tonumber( L, -1 );
	
	
	glBegin( GL_LINES );
		glVertex2f( a, b );
		glVertex2f( x, y );
	glEnd();
	
		//char caDbg[1024];
		//sprintf( caDbg, "Gizmo: gfx.DrawLine( %0.3f,%0.3f,   %0.3f,%0.3f ) ", a,b,x,y );
		//XPLMDebugString( caDbg );
	
	return 0;
	
}


/**
gfx.drawFilledBox( left,bottom,width,height )
 
 Draws a box using raw OpenGL functions.
 Use gfx.setColor( r,g,b, a ) to control color.
 User gfx.texOff() to turn texturing on or off.
 
 */
int gizmo_builtin_gfx_drawFilledBox(lua_State *L){

	if( lua_gettop( L ) != 4 ){
		gizmo_gui_ScriptError("gfx.drawFilledBox() - wrong number of arguments. See docs.");
		return 0;
	}
	
	
		lua_Number lnVx = lua_tonumber( L, -4 ); //l
		lua_Number lnVy = lua_tonumber( L, -3 ); //b
		lua_Number lnVw = lua_tonumber( L, -2 ); //w
		lua_Number lnVh = lua_tonumber( L, -1 ); //h

			glBegin( GL_POLYGON );
				//clockwise winding..
				glVertex2f( lnVx, lnVy ); //l,b
					glVertex2f( lnVx, lnVy+lnVh ); //l,t
					glVertex2f( lnVx+lnVw, lnVy+lnVh ); //r,t
				glVertex2f( lnVx+lnVw, lnVy ); //r,b
			glEnd();

	return 0;

}



/**
gfx.drawRadial( heading, radius, length )

 Draws a radial line, as used on gauge faces for tick marks, properly rotated to radiate from center.
 */
int gizmo_builtin_gfx_drawRadial(lua_State *L){

	if( lua_gettop( L ) != 3 ){
		gizmo_gui_ScriptError("gfx.drawRadial() - wrong number of arguments. See docs.");
		return 0;
	}
	
	
		lua_Number lnVheading = lua_tonumber( L, -3 );
		lua_Number lnVradius = lua_tonumber( L, -2 );
		lua_Number lnVlength = lua_tonumber( L, -1 );


			float fRadialX1, fRadialY1;
			float fRadialX2, fRadialY2;

			glBegin( GL_LINES );

					XTrig::computeNewPositionOffset( lnVheading, lnVradius, &fRadialX1, &fRadialY1 );
					XTrig::computeNewPositionOffset( lnVheading, lnVradius+lnVlength, &fRadialX2, &fRadialY2 );
					glVertex2f( fRadialX1, -fRadialY1 );
					glVertex2f( fRadialX2, -fRadialY2 );

			glEnd();

	return 0;

}






/**
 gfx.bakeTerrainMap( texture_id )
 Bakes a terrain texture image into the desired texture ID. Drawing is handled by your own code.
 */
int gizmo_builtin_gfx_bakeTerrainMap(lua_State *L){

	if( lua_gettop( L ) != 1 ){
		gizmo_gui_ScriptError("gfx.bakeTerrainMap() - wrong number of arguments. See docs.");
		return 0;
	}
	
	
	lua_Number lnTexture = lua_tonumber( L, -1 );

		gizmo_Mapping_TerrainBake( lnTexture );

		//this function is sharded.
		gizmo_ProbeTerrain( 0 ); //command line arg is "force full update"

	return 0;
}



/**
 gfx.setTerrainMapMode_Predictive()
 
 Switch the coloring mode of the terrain map renderer into predictive color rendering.
 */
int gizmo_builtin_gfx_setTerrainMapMode_Predictive(lua_State *L){
	gizmo_SetTerrainMode_Predictive();
	return 0;
}


/**
 gfx.setTerrainMapMode_Relative()
 Switch the coloring mode of the terrain map renderer into relative color rendering.
 */
int gizmo_builtin_gfx_setTerrainMapMode_Relative(lua_State *L){
	gizmo_SetTerrainMode_Relative();
	return 0;
}


/**
 gfx.setTerrainMapMode_Synthetic()
 Switch the coloring mode of the terrain map renderer into Synthetic color rendering.
 */
int gizmo_builtin_gfx_setTerrainMapMode_Synthetic(lua_State *L){
	gizmo_SetTerrainMode_Synthetic();
	return 0;
}


/**
 gfx.setTerrainMapMode_Topographic()
 Switch the coloring mode of the terrain map renderer into FAA topographic color rendering.
 */
int gizmo_builtin_gfx_setTerrainMapMode_Topographic(lua_State *L){
	gizmo_SetTerrainMode_Topographic();
	return 0;
}



/**
 gfx.setTerrainMapOption_LookAhead( integer_flag )
 Puts the map into "offset scan" mode, the airframe is approx 1/3 from the bottom edge of the map.
 */
int gizmo_builtin_gfx_setTerrainMapOption_LookAhead(lua_State *L){
	lua_Number lnV = lua_tonumber( L, -1 );
	gizmo_SetTerrainOption_LookAhead( (int)lnV );
	return 0;
}


/**
 gfx.setTerrainMapOption_NorthUp( integer_flag )
 Locks the map to North-Up mode, you will need to render the users vehicle symbol rotating for the map to make much sense.
 */
int gizmo_builtin_gfx_setTerrainMapOption_NorthUp(lua_State *L){
	lua_Number lnV = lua_tonumber( L, -1 );
	gizmo_SetTerrainOption_NorthUp( (int)lnV );
	return 0;
}


/**
 gfx.setTerrainMapOption_Range( meters_between_sample_points )
 range * size = scan area.
 */
int gizmo_builtin_gfx_setTerrainMapOption_Range(lua_State *L){
	lua_Number lnV = lua_tonumber( L, -1 );
	gizmo_SetTerrainOption_Range( (float)lnV );
	return 0;
}


/**
 gfx.setTerrainMapOption_RefreshRate( rate_value )
 How agressively do we try and build the map data. 
 CONFIRM: I think this controls how many scan-lines of the map we try and generate per sim frame.
 The map system is sliced into shards so that low end machines dont "chug" when the map is sampling.
 */
int gizmo_builtin_gfx_setTerrainMapOption_RefreshRate(lua_State *L){
	lua_Number lnV = lua_tonumber( L, -1 );
	gizmo_SetTerrainOption_RefreshRate( (int)lnV );
	return 0;
}



/**
 gfx.setTerrainMapOption_Size( number_of_samples_along_an_edge )
 scan area for the map = size * range
 This directly affects the OpenGL texture size. Default is 128. 
 Valid values: 32,64,128,256,512,1024 etc
 Map texture is sampled and rendered "square".
 */
int gizmo_builtin_gfx_setTerrainMapOption_Size(lua_State *L){
	lua_Number lnV = lua_tonumber( L, -1 );
	gizmo_SetTerrainOption_Size( (int)lnV );
	return 0;
}



/**
 gfx.drawObject( object_id, lat,lon,alt, p,r,h )
 Draw an object in World Coordinates. Lat/Lon/Alt.
 */
int gizmo_builtin_gfx_drawObject(lua_State *L){

	lua_Integer lnObjectID = lua_tointeger( L, -7 );

		lua_Number lnLat = lua_tonumber( L, -6 );
		lua_Number lnLon = lua_tonumber( L, -5 );
		lua_Number lnAlt = lua_tonumber( L, -4 );

		lua_Number lnPitch = lua_tonumber( L, -3 );
		lua_Number lnRoll = lua_tonumber( L, -2 );
		lua_Number lnHeading = lua_tonumber( L, -1 );


	double dX, dY, dZ;
	XPLMWorldToLocal( lnLat, lnLon, lnAlt, &dX, &dY, &dZ );


		//Add this item to the gizmo drawing Queue
		gizmo_QDrawObject (
							  (int)lnObjectID,
							  (float)dX,
							  (float)dY,
							  (float)dZ,
							  (float)lnPitch,
							  (float)lnRoll,
							  (float)lnHeading
						  );


	return 0;
}


/**
 gfx.drawObjectGL( object_handle, x,y,z, p,r,h )
 Draws using OpenGL coords.
 */
int gizmo_builtin_gfx_drawObjectGL(lua_State *L){

	lua_Integer lnObjectID = lua_tointeger( L, -7 );

		lua_Number lnX = lua_tonumber( L, -6 );
		lua_Number lnY = lua_tonumber( L, -5 );
		lua_Number lnZ = lua_tonumber( L, -4 );

		lua_Number lnPitch = lua_tonumber( L, -3 );
		lua_Number lnRoll = lua_tonumber( L, -2 );
		lua_Number lnHeading = lua_tonumber( L, -1 );


	//printf("Gizmo: gfx.drawObjectGL: %i - %0.3f %0.3f %0.3f - %0.3f %0.3f %0.3f\n", lnObjectID, lnX, lnY, lnZ, lnPitch, lnHeading, lnRoll );


		//Add this item to the gizmo drawing Queue
		gizmo_QDrawObject (
							  (int)lnObjectID,
							  (float)lnX,
							  (float)lnY,
							  (float)lnZ,
							  (float)lnPitch,
							  (float)lnRoll,
							  (float)lnHeading
						  );


	return 0;
}




/**
 gfx.drawObjectEx( object_handle, x,y,z, p,r,h )
 Draws using X-Planes OBJ8 calls _immediately_, does not force you through Gizmos Queue.
 */
int gizmo_builtin_gfx_drawObjectEx(lua_State *L){

	lua_Integer lnObjectID = lua_tointeger( L, -7 );

		lua_Number lnX = lua_tonumber( L, -6 );
		lua_Number lnY = lua_tonumber( L, -5 );
		lua_Number lnZ = lua_tonumber( L, -4 );

		lua_Number lnPitch = lua_tonumber( L, -3 );
		lua_Number lnRoll = lua_tonumber( L, -2 );
		lua_Number lnHeading = lua_tonumber( L, -1 );

	XPLMSetGraphicsState(
						 1, //fog
						 1, //num tex units
						 1, //lighting
						 1, //alpha test
						 1, //alpha blend
						 1, //depth test
						 1  //depth write
						 ); //very important!



	XPLMDrawInfo_t drawLocation[1];// = new XPLMDrawInfo_t[1];
					drawLocation[0].structSize = sizeof( XPLMDrawInfo_t );
					drawLocation[0].x 		= (float)lnX;
					drawLocation[0].y 		= (float)lnY;
					drawLocation[0].z 		= (float)lnZ;
					drawLocation[0].pitch 	= (float)lnPitch;
					drawLocation[0].roll 	= (float)lnRoll;
					drawLocation[0].heading = (float)lnHeading;


	XPLMObjectRef tmpObj;

			tmpObj = gizmo_XPlaneSceneryObjectHandles[ (int)lnObjectID ];

			XPLMDrawObjects(
							tmpObj,
							1,
							drawLocation, //gizmo_DrawQ[x].DrawInfo, //array of drawing locations
							0, //lighting
							1	//earth relative
							);

	return 0;
}


/**
 gfx.worldToLocal( lat,lon,alt )
 Convert world-space coordinates, from a real map, into OpenGL drawing coordinates, for use in a Game Engine.
 This function makes no attempt to check if your request makes sense, if you ask for coordinates on the other side of the planet the result is undefined. Good luck.
 x,y,z = gfx.worldToLocal( lat,lon,alt )
 */
int gizmo_builtin_gfx_worldToLocal(lua_State *L){

	//Minor, variable names dont match actual values properly, have not changed XYZ <-> LLA.
	//Functionaly code is fine.

		lua_Number lnVX = lua_tonumber( L, -3 );
		lua_Number lnVY = lua_tonumber( L, -2 );
		lua_Number lnVZ = lua_tonumber( L, -1 );

		double dLat, dLon, dAlt;

		XPLMWorldToLocal( (double)lnVX, (double)lnVY, (double)lnVZ,
							&dLat, &dLon, &dAlt );

		lua_pushnumber(L, dLat);
		lua_pushnumber(L, dLon);
		lua_pushnumber(L, dAlt);

	return 3;

}
