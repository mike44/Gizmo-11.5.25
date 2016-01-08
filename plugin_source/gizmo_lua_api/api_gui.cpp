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
 gui.blur( gui_window_id )
 FIXME: gitbug:13 Does nothing: gui.blur()
 */
int gizmo_builtin_gui_blur(lua_State *L){
	/*
	XPUDefocusKeyboard(
		   XPWidgetMessage      inMessage,    
		   XPWidgetID           inWidget,    
		   long                 inParam1,    
		   long                 inParam2,    
		   int                  inEatClick)
	*/
	return 0;
}


/**
 gui.getCustomWidgetPosition( gui_widget_id )
 Returns: left,top
 */
int gizmo_builtin_gui_getCustomWidgetPosition(lua_State *L){
	lua_pushnumber(L, gizmo_gui_lastCustomWidgetLeft);
	lua_pushnumber(L, gizmo_gui_lastCustomWidgetBottom);
	
	return 2;
}


/**
 gui.getCustomWidgetSize( gui_widget_id )
 Returns: width,height
 */
int gizmo_builtin_gui_getCustomWidgetSize(lua_State *L){
	lua_pushnumber(L, gizmo_gui_lastCustomWidgetWidth);
	lua_pushnumber(L, gizmo_gui_lastCustomWidgetHeight);
	
	return 2;
}


/**
 gui.getCustomWidgetMouseXY( gui_widget_id )
 Returns: x,y for mouse corrected for use in Custom Widget graphics.
 
 API: > 11.5.15
 */
int gizmo_builtin_gui_getCustomWidgetMouseXY(lua_State *L){
	
	int x, y;
	XPLMGetMouseLocation( &x, &y );
	
	//gizmo_gui_lastCustomWidgetLeft
	//gizmo_gui_lastCustomWidgetBottom
	
	lua_pushnumber( L, x - gizmo_gui_lastCustomWidgetLeft );
	lua_pushnumber( L, (y - gizmo_gui_lastCustomWidgetBottom)+2 );
	
	return 2;
}




/**
 gui.getWidgetValue( gui_widget_id )
 Returns: value of gui_widget_id
 */
int gizmo_builtin_gui_getWidgetValue(lua_State *L){

	lua_Integer lnV_WidgetID = lua_tointeger( L, -1 );
		char *outBuffer = new char[8192];
			XPGetWidgetDescriptor( gizmo_Pool_BaseWrappers[ lnV_WidgetID ]->WidgetID, outBuffer, 8192);
			lua_pushstring( L, outBuffer );
		delete( outBuffer );

	return 1;
}




/**
 gui.setPasswordMode( gui_widget_id, int_flag )
 Control the password-hiding mode of a text field.
 */
int gizmo_builtin_gui_setPasswordMode(lua_State *L){
	lua_Integer lnV_WidgetID	= lua_tointeger( L, -2 );
	lua_Integer lnV_Flag		= lua_tointeger( L, -1 );

		if( gizmo_Pool_BaseWrappers[ lnV_WidgetID ]->XPlaneWidgetType == xpWidgetClass_TextField ){

			XPSetWidgetProperty( gizmo_Pool_BaseWrappers[ lnV_WidgetID ]->WidgetID, xpProperty_PasswordMode, lnV_Flag );

		}
		
	return 0;
}



/**
 gui.setWidgetValue( gui_widget_id, "new value" )
 Sets the value of the target widget to the value you pass in.
 Values can be strings or numbers, other data will probably fail.
 */
int gizmo_builtin_gui_setWidgetValue(lua_State *L){
	size_t szLen;
	lua_Integer lnV_WidgetID = lua_tointeger( L, -2 );
	const char *sInput = lua_tolstring( L, -1, &szLen );

			XPSetWidgetDescriptor( gizmo_Pool_BaseWrappers[ lnV_WidgetID ]->WidgetID, sInput);

	return 0;
}




/**
 gui.getWindowList()
 Returns: table of strings containing the names of all GUI Windows Gizmo is managing.
 window_list = { gui.getWindowList() }
 */
int gizmo_builtin_gui_getWindowList(lua_State *L){

	for( unsigned int x=0; x < gizmo_Pool_WindowWrappers.size(); x++ ){
		lua_pushstring( L, gizmo_Pool_WindowWrappers[ x ]->WindowCaption.c_str() );
	}

	return gizmo_Pool_WindowWrappers.size();
}




/**
 gui.hideWidget( gui_widget_id )
 */
int gizmo_builtin_gui_hideWidget(lua_State *L){

	lua_Integer lnV_WidgetID = lua_tointeger( L, -1 );

	if( lnV_WidgetID < (int)gizmo_Pool_BaseWrappers.size() ){
		XPHideWidget( (gizmo_Pool_BaseWrappers[lnV_WidgetID]->WidgetID) );
	}
	
	return 0;
}




/**
 gui.hideWindow( gui_window_id )
 */
int gizmo_builtin_gui_hideWindow(lua_State *L){

	lua_Integer lnV = lua_tointeger( L, -1 );

	if( lnV < (int)gizmo_Pool_WindowWrappers.size() ){
		gizmo_Pool_WindowWrappers[lnV]->Hide();
	}

	//lua_pushnumber(L, 1);

	return 0;
}




/**
 gui.setWindowCaption( gui_window_id, "My Window Caption" )
 */
int gizmo_builtin_gui_setWindowCaption(lua_State *L){


	lua_Integer lnV_WindowID	= lua_tointeger( L, -2 );

	size_t szLen;
	const char *sInput = lua_tolstring( L, -1, &szLen );


		XPLMAppendMenuItem(
									gizmo_Menu_Windows,
									sInput,

									(void *)gizmo_Pool_WindowWrappers[ lnV_WindowID ]->WindowIdentifier.c_str(),
									//(void *)sInput_WindowIdentifier, //item ref

									1 //force english
									);


							  gizmo_Pool_WindowWrappers[ lnV_WindowID ]->WindowCaption = std::string(sInput);
		XPSetWidgetDescriptor(gizmo_Pool_WindowWrappers[ lnV_WindowID ]->WidgetID, sInput);

	return 0;
}




/**
 gui.setWindowSize( gui_window_id, w, h )
 */
int gizmo_builtin_gui_setWindowSize(lua_State *L){

	lua_Integer lnV_WindowID = lua_tointeger( L, -5 );

	lua_Integer lnV_Left	= lua_tointeger( L, -4 );
	lua_Integer lnV_Top		= lua_tointeger( L, -3 );
	lua_Integer lnV_Width	= lua_tointeger( L, -2 );
	lua_Integer lnV_Height	= lua_tointeger( L, -1 );

		gizmo_Pool_WindowWrappers[ lnV_WindowID ]->Left = lnV_Left;

		//coord system is reversed from SDK default.
		//SDK uses bottom left as 0 point. Gizmo uses top left. This is the same as HTML and slightly easier to work with.
		//child widgets are also automatically placed in this way with the parent widget-window defining the 0 point at its top left.
		gizmo_Pool_WindowWrappers[ lnV_WindowID ]->Top = XPLMGetDatai( gizmo_xpdr_ScreenHeight ) - (int)lnV_Top;

		gizmo_Pool_WindowWrappers[ lnV_WindowID ]->Width = lnV_Width;
		gizmo_Pool_WindowWrappers[ lnV_WindowID ]->Height = lnV_Height;

		gizmo_Pool_WindowWrappers[ lnV_WindowID ]->Right = gizmo_Pool_WindowWrappers[ lnV_WindowID ]->Left + gizmo_Pool_WindowWrappers[ lnV_WindowID ]->Width;
		gizmo_Pool_WindowWrappers[ lnV_WindowID ]->Bottom = gizmo_Pool_WindowWrappers[ lnV_WindowID ]->Top - gizmo_Pool_WindowWrappers[ lnV_WindowID ]->Height;


		XPSetWidgetGeometry( gizmo_Pool_WindowWrappers[ lnV_WindowID ]->WidgetID,
							gizmo_Pool_WindowWrappers[ lnV_WindowID ]->Left,
							gizmo_Pool_WindowWrappers[ lnV_WindowID ]->Top,
							gizmo_Pool_WindowWrappers[ lnV_WindowID ]->Right,
							gizmo_Pool_WindowWrappers[ lnV_WindowID ]->Bottom
							);


	return 0;
}



/**
 gui.showWidget( gui_widget_id )
 Show target Widget.
 */
int gizmo_builtin_gui_showWidget(lua_State *L){

	lua_Integer lnV_WidgetID = lua_tointeger( L, -1 );

	if( lnV_WidgetID < (int)gizmo_Pool_BaseWrappers.size() ){
		XPShowWidget( (gizmo_Pool_BaseWrappers[lnV_WidgetID]->WidgetID) );
	}
	
	return 0;
}



/**
 gui.showWindow( gui_window_id )
 Show target Window.
 */
int gizmo_builtin_gui_showWindow(lua_State *L){

	lua_Integer lnV = lua_tointeger( L, -1 );

	if( lnV < (int)gizmo_Pool_WindowWrappers.size() ){
		gizmo_Pool_WindowWrappers[lnV]->Show();
	}

	return 0;
}





/**
 gui.newButton( gui_window_id, "MyClickHandlerFunction", "Button Caption", left, top, width )
 Returns: gui_widget_id
 When clicked the function MyClickHandlerFunction_OnClick() will be called.
 */
int gizmo_builtin_gui_newButton(lua_State *L){

	int n = lua_gettop(L);
	if( n < 6 ){ 
		XPLMDebugString("Gizmo: ERROR: Call to gui.newButton() failed, not enough parameters.\n");
		return 0; 
	} //MUST have 6 args, or ignored.

	

	size_t szLen;
	lua_Number lnV_ParentWindowID = lua_tonumber( L, -6 );

	const char *sInput_ScriptHandler = lua_tolstring( L, -5, &szLen );
	const char *sInput_WindowCaption = lua_tolstring( L, -4, &szLen );

	lua_Number lnV_Left = lua_tonumber( L, -3 );
	lua_Number lnV_Top = lua_tonumber( L, -2 );
	lua_Number lnV_Width = lua_tonumber( L, -1 );

	#if 0
		char caDbg[2048];
		sprintf(caDbg, "Gizmo: gui.newButton( %i, %s, %s )\n", (int)lnV_ParentWindowID, sInput_ScriptHandler, sInput_WindowCaption);
		XPLMDebugString(caDbg);
	#endif


	XLuaWidgetBaseWrapper *tmpWidget = new XLuaWidgetBaseWrapper(
																	gizmo_Pool_WindowWrappers[ (int)lnV_ParentWindowID ],
																	std::string( sInput_ScriptHandler ), //handler
																	std::string( sInput_WindowCaption ), //caption
																	(int)lnV_Left, (int)lnV_Top, //left, top
																	(int)lnV_Width,
																	15, //** height forced - lnV_Height, //width, height
																	1 //visible
																);
							tmpWidget->CreateAsButton();


		//add default callback function
		XPAddWidgetCallback( tmpWidget->WidgetID, gizmo_Callback_gui_WidgetHandler);



	int iWrapperPoolID = gizmo_Pool_BaseWrappers.size();
	gizmo_Pool_BaseWrappers.push_back( tmpWidget );


	lua_pushnumber(L, iWrapperPoolID);

	return 1;
}







/**
 gui.newCheckbox( gui_window_id, "MyClickHandlerFunction", "Button Caption", left, top, width )
 Returns: gui_widget_id
 When clicked the function MyClickHandlerFunction_OnClick() will be called.
 */
int gizmo_builtin_gui_newCheckbox(lua_State *L){

	int n = lua_gettop(L);
	if( n < 6 ){ 
		XPLMDebugString("Gizmo: ERROR: Call to gui.newCheckbox() failed, not enough parameters.\n");
		return 0; 
	} //MUST have 6 args, or ignored.

	


	size_t szLen;
	lua_Number lnV_ParentWindowID = lua_tonumber( L, -6 );

	const char *sInput_ScriptHandler = lua_tolstring( L, -5, &szLen );
	const char *sInput_WindowCaption = lua_tolstring( L, -4, &szLen );

	lua_Number lnV_Left = lua_tonumber( L, -3 );
	lua_Number lnV_Top = lua_tonumber( L, -2 );
	lua_Number lnV_Width = lua_tonumber( L, -1 );

	#if 0
		char caDbg[2048];
		sprintf(caDbg, "Gizmo: gui.newCheckbox( %i, %s, %s )\n", (int)lnV_ParentWindowID, sInput_ScriptHandler, sInput_WindowCaption);
		XPLMDebugString(caDbg);
	#endif

	XLuaWidgetBaseWrapper *tmpWidget = new XLuaWidgetBaseWrapper(
																	gizmo_Pool_WindowWrappers[ (int)lnV_ParentWindowID ],
																	std::string( sInput_ScriptHandler ), //handler
																	std::string( sInput_WindowCaption ), //caption
																	(int)lnV_Left, (int)lnV_Top, //left, top
																	(int)lnV_Width,
																	15, //** height forced - lnV_Height, //width, height
																	1 //visible
																);
							tmpWidget->CreateAsCheckbox();


		//add default callback function
		XPAddWidgetCallback( tmpWidget->WidgetID, gizmo_Callback_gui_WidgetHandler);



	int iWrapperPoolID = gizmo_Pool_BaseWrappers.size();
	gizmo_Pool_BaseWrappers.push_back( tmpWidget );


	lua_pushnumber(L, iWrapperPoolID);

	return 1;
}








/**
 gui.newCustomWidget( gui_window_id, "EventHandlerFunction", left, top, width )
 Returns: gui_widget_id
 */
int gizmo_builtin_gui_newCustomWidget(lua_State *L){


	int n = lua_gettop(L);
	if( n < 6 ){ 
		XPLMDebugString("Gizmo: ERROR: Call to gui.newCustomWidget() failed, not enough parameters.\n");
		return 0; 
	} //MUST have 6 args, or ignored.

	



	size_t szLen;
	lua_Number lnV_ParentWindowID = lua_tonumber( L, -6 );

	const char *sInput_ScriptHandler = lua_tolstring( L, -5, &szLen );
	//const char *sInput_WindowCaption = lua_tolstring( L, -5, &szLen );

	lua_Number lnV_Left = lua_tonumber( L, -4 );
	lua_Number lnV_Top = lua_tonumber( L, -3 );
	lua_Number lnV_Width = lua_tonumber( L, -2 );
	lua_Number lnV_Height = lua_tonumber( L, -1 );

	#if 0
		char caDbg[2048];
		sprintf(caDbg, "Gizmo: gui.newCustomWidget( %i, '%s', %i )\n", (int)lnV_ParentWindowID, sInput_ScriptHandler, (int)gizmo_Pool_WindowWrappers[ (int)lnV_ParentWindowID ]->WidgetID );
		XPLMDebugString(caDbg);
	#endif


	lnV_Left += gizmo_Pool_WindowWrappers[ (int)lnV_ParentWindowID ]->Left;
	lnV_Top = gizmo_Pool_WindowWrappers[ (int)lnV_ParentWindowID ]->Top - lnV_Top;
	

		
		//attempting to track custom widgets.....
	



		XPWidgetID tmpWidgetHandle = XPCreateCustomWidget(
						lnV_Left,				//left
						lnV_Top,				//top
						lnV_Left + lnV_Width, //right
						lnV_Top - lnV_Height, //bottom
						
						1,						//visible
						sInput_ScriptHandler, //descriptor										
						0,						//is root widget
						
						gizmo_Pool_WindowWrappers[ (int)lnV_ParentWindowID ]->WidgetID, //parent widget
						gizmo_Callback_gui_CustomWidgetHandler //custom callback handler function
						);



		//clipping.
		XPSetWidgetProperty(
							tmpWidgetHandle,
							xpProperty_Clip,
							1
							);
							


		XLuaWidgetBaseWrapper *tmpWidget = new XLuaWidgetBaseWrapper(
																	gizmo_Pool_WindowWrappers[ (int)lnV_ParentWindowID ],
																	std::string( sInput_ScriptHandler ), //handler
																	std::string( "" ), //caption
																	(int)lnV_Left, (int)lnV_Top, //left, top
																	(int)lnV_Width,
																	(int)lnV_Height, //** height forced: lnV_Height, //width, height
																	1 //visible
																);
		tmpWidget->WidgetID = tmpWidgetHandle;


	//store a handle for cleanup on scripts reboot.
	int iWrapperPoolID = gizmo_Pool_BaseWrappers.size();
	gizmo_Pool_BaseWrappers.push_back( tmpWidget );


	lua_pushnumber(L, iWrapperPoolID);
	return 1;

}






/**
 gui.newLabel( gui_window_id, "EventHandlerFunction", "Button Caption", left, top, width )
 Returns: gui_widget_id
 */
int gizmo_builtin_gui_newLabel(lua_State *L){


	int n = lua_gettop(L);
	if( n < 6 ){ 
		XPLMDebugString("Gizmo: ERROR: Call to gui.newLabel() failed, not enough parameters.\n");
		return 0; 
	} //MUST have 6 args, or ignored.

	



	size_t szLen;
	lua_Number lnV_ParentWindowID = lua_tonumber( L, -6 );

	const char *sInput_ScriptHandler = lua_tolstring( L, -5, &szLen );
	const char *sInput_WindowCaption = lua_tolstring( L, -4, &szLen );

	lua_Number lnV_Left = lua_tonumber( L, -3 );
	lua_Number lnV_Top = lua_tonumber( L, -2 );
	lua_Number lnV_Width = lua_tonumber( L, -1 );
	//lua_Number lnV_Height = lua_tonumber( L, -1 );


		#if 0
			{
			char caDbg[2048];
			sprintf(caDbg,"Gizmo: gui.newLabel( %i, %s, %s )\n", (int)lnV_ParentWindowID, sInput_ScriptHandler, sInput_WindowCaption);
			XPLMDebugString(caDbg);
			}
		#endif



	XLuaWidgetBaseWrapper *tmpWidget = new XLuaWidgetBaseWrapper(
																	gizmo_Pool_WindowWrappers[ (int)lnV_ParentWindowID ],
																	std::string( sInput_ScriptHandler ), //handler
																	std::string( sInput_WindowCaption ), //caption
																	(int)lnV_Left, (int)lnV_Top-2, //left, top
																	(int)lnV_Width,
																	15, //** height forced: lnV_Height, //width, height
																	1 //visible
																);
							tmpWidget->CreateAsLabel();


	int iWrapperPoolID = gizmo_Pool_BaseWrappers.size();
	gizmo_Pool_BaseWrappers.push_back( tmpWidget );


	lua_pushnumber(L, iWrapperPoolID);

	return 1;
}





/**
 gui.newListView( gui_window_id, "EventHandlerFunction", "Button Caption", left, top, width )
 Returns: gui_widget_id
 */
int gizmo_builtin_gui_newListView(lua_State *L){


	int n = lua_gettop(L);
	if( n < 6 ){ 
		XPLMDebugString("Gizmo: ERROR: Call to gui.newListView() failed, not enough parameters.\n");
		return 0; 
	} //MUST have 6 args, or ignored.

	



	size_t szLen;
	lua_Number lnV_ParentWindowID = lua_tonumber( L, -6 );

	const char *sInput_ScriptHandler = lua_tolstring( L, -5, &szLen );

	lua_Number lnV_Left = lua_tonumber( L, -4 );
	lua_Number lnV_Top = lua_tonumber( L, -3 );
	lua_Number lnV_Width = lua_tonumber( L, -2 );
	lua_Number lnV_Height = lua_tonumber( L, -1 );


		#if 0
			{
			char caDbg[2048];
			sprintf(caDbg,"Gizmo: gui.newListView( %i, %s, %s )\n", (int)lnV_ParentWindowID, sInput_ScriptHandler, sInput_WindowCaption);
			XPLMDebugString(caDbg);
			}
		#endif



	XLuaWidgetBaseWrapper *tmpWidget = new XLuaWidgetBaseWrapper(
																	gizmo_Pool_WindowWrappers[ (int)lnV_ParentWindowID ],
																	std::string( sInput_ScriptHandler ), //handler
																	std::string( "" ), //caption
																	(int)lnV_Left, (int)lnV_Top-2, //left, top
																	(int)lnV_Width,
																	(int)lnV_Height,
																	1 //visible
																);
							tmpWidget->CreateAsListView();


	int iWrapperPoolID = gizmo_Pool_BaseWrappers.size();
	gizmo_Pool_BaseWrappers.push_back( tmpWidget );


	lua_pushnumber(L, iWrapperPoolID);

	return 1;
}






/**
 gui.newRadioButton( gui_window_id, "MyClickHandlerFunction", "Button Caption", left, top, width )
 Returns: gui_widget_id
 When clicked the function MyClickHandlerFunction_OnClick() will be called.
 */
int gizmo_builtin_gui_newRadioButton(lua_State *L){

	int n = lua_gettop(L);
	if( n < 6 ){ 
		XPLMDebugString("Gizmo: ERROR: Call to gui.newRadioButton() failed, not enough parameters.\n");
		return 0; 
	} //MUST have 6 args, or ignored.

	
	size_t szLen;
	lua_Number lnV_ParentWindowID = lua_tonumber( L, -6 );

	const char *sInput_ScriptHandler = lua_tolstring( L, -5, &szLen );
	const char *sInput_WindowCaption = lua_tolstring( L, -4, &szLen );

	lua_Number lnV_Left = lua_tonumber( L, -3 );
	lua_Number lnV_Top = lua_tonumber( L, -2 );
	lua_Number lnV_Width = lua_tonumber( L, -1 );

	
	char caDbg[2048];
	sprintf(caDbg, "Gizmo: gui.newRadioButton( %i, %s, %s )\n", (int)lnV_ParentWindowID, sInput_ScriptHandler, sInput_WindowCaption);
	//XPLMDebugString(caDbg);


	XLuaWidgetBaseWrapper *tmpWidget = new XLuaWidgetBaseWrapper(
																	gizmo_Pool_WindowWrappers[ (int)lnV_ParentWindowID ],
																	std::string( sInput_ScriptHandler ), //handler
																	std::string( sInput_WindowCaption ), //caption
																	(int)lnV_Left, (int)lnV_Top, //left, top
																	(int)lnV_Width,
																	15, //** height forced - lnV_Height, //width, height
																	1 //visible
																);
							tmpWidget->CreateAsRadioButton();


		//add default callback function
		XPAddWidgetCallback( tmpWidget->WidgetID, gizmo_Callback_gui_WidgetHandler);



	int iWrapperPoolID = gizmo_Pool_BaseWrappers.size();
	gizmo_Pool_BaseWrappers.push_back( tmpWidget );


	lua_pushnumber(L, iWrapperPoolID);

	return 1;
}





/**
 gui.newSubWindow( gui_window_id, "MyClickHandlerFunction", "Button Caption", left, top, width )
 Returns: gui_widget_id
 When clicked the function MyClickHandlerFunction_OnClick() will be called.
 */
int gizmo_builtin_gui_newSubWindow(lua_State *L){


	int n = lua_gettop(L);
	if( n < 6 ){ 
		XPLMDebugString("Gizmo: ERROR: Call to gui.newSubWindow() failed, not enough parameters.\n");
		return 0; 
	} //MUST have 6 args, or ignored.


	size_t szLen;
	lua_Number lnV_ParentWindowID = lua_tonumber( L, -6 );

	const char *sInput_ScriptHandler = lua_tolstring( L, -5, &szLen );

	lua_Number lnV_Left = lua_tonumber( L, -4 );
	lua_Number lnV_Top = lua_tonumber( L, -3 );
	lua_Number lnV_Width = lua_tonumber( L, -2 );
	lua_Number lnV_Height = lua_tonumber( L, -1 );


		#if 0
			{
			char caDbg[2048];
			sprintf(caDbg,"Gizmo: gui.newSubWindow( %i, %s, %s )\n", (int)lnV_ParentWindowID, sInput_ScriptHandler, sInput_WindowCaption);
			XPLMDebugString(caDbg);
			}
		#endif



	XLuaWidgetBaseWrapper *tmpWidget = new XLuaWidgetBaseWrapper(
																	gizmo_Pool_WindowWrappers[ (int)lnV_ParentWindowID ],
																	std::string( sInput_ScriptHandler ), //handler
																	std::string( "" ), //caption
																	(int)lnV_Left, (int)lnV_Top-2, //left, top
																	(int)lnV_Width,
																	(int)lnV_Height,
																	1 //visible
																);
							tmpWidget->CreateAsSubWindow();


	int iWrapperPoolID = gizmo_Pool_BaseWrappers.size();
	gizmo_Pool_BaseWrappers.push_back( tmpWidget );


	lua_pushnumber(L, iWrapperPoolID);

	return 1;
}



/**
 gui.newScreen( gui_window_id, "MyClickHandlerFunction", "Button Caption", left, top, width )
 Returns: gui_widget_id
 When clicked the function MyClickHandlerFunction_OnClick() will be called.
 I think this is for some kind of SDK-named sub-window. The naming sucks. Totally ambiguous/confusing.
 */
int gizmo_builtin_gui_newScreen(lua_State *L){


	int n = lua_gettop(L);
	if( n < 6 ){ 
		XPLMDebugString("Gizmo: ERROR: Call to gui.newScreen() failed, not enough parameters.\n");
		return 0; 
	} //MUST have 6 args, or ignored.


	size_t szLen;
	lua_Number lnV_ParentWindowID = lua_tonumber( L, -6 );

	const char *sInput_ScriptHandler = lua_tolstring( L, -5, &szLen );

	lua_Number lnV_Left = lua_tonumber( L, -4 );
	lua_Number lnV_Top = lua_tonumber( L, -3 );
	lua_Number lnV_Width = lua_tonumber( L, -2 );
	lua_Number lnV_Height = lua_tonumber( L, -1 );


		#if 0
			{
			char caDbg[2048];
			sprintf(caDbg,"Gizmo: gui.newScreen( %i, %s, %s )\n", (int)lnV_ParentWindowID, sInput_ScriptHandler, sInput_WindowCaption);
			XPLMDebugString(caDbg);
			}
		#endif



	XLuaWidgetBaseWrapper *tmpWidget = new XLuaWidgetBaseWrapper(
																	gizmo_Pool_WindowWrappers[ (int)lnV_ParentWindowID ],
																	std::string( sInput_ScriptHandler ), //handler
																	std::string( "" ), //caption
																	(int)lnV_Left, (int)lnV_Top-2, //left, top
																	(int)lnV_Width,
																	(int)lnV_Height,
																	1 //visible
																);
							tmpWidget->CreateAsScreen();


	int iWrapperPoolID = gizmo_Pool_BaseWrappers.size();
	gizmo_Pool_BaseWrappers.push_back( tmpWidget );


	lua_pushnumber(L, iWrapperPoolID);

	return 1;
}



/**
 gui.newTextBox( gui_window_id, "EventHandlerFunction", "Box Contents", left, top, width )
 Returns: gui_widget_id
 */
int gizmo_builtin_gui_newTextBox(lua_State *L){

	int n = lua_gettop(L);
	if( n < 6 ){ 
		XPLMDebugString("Gizmo: ERROR: Call to gui.newTextBox() failed, not enough parameters.\n");
		return 0; 
	} //MUST have 6 args, or ignored.

	

	size_t szLen;
	lua_Number lnV_ParentWindowID = lua_tonumber( L, -6 );

	const char *sInput_ScriptHandler = lua_tolstring( L, -5, &szLen );
	const char *sInput_WindowCaption = lua_tolstring( L, -4, &szLen );

	lua_Number lnV_Left = lua_tonumber( L, -3 );
	lua_Number lnV_Top = lua_tonumber( L, -2 );
	lua_Number lnV_Width = lua_tonumber( L, -1 );
	//lua_Number lnV_Height = lua_tonumber( L, -1 );


	char caDbg[2048];
	sprintf(caDbg, "Gizmo: gui.newTextBox( %i, %s, %s )\n", (int)lnV_ParentWindowID, sInput_ScriptHandler, sInput_WindowCaption);
	//XPLMDebugString(caDbg);



	XLuaWidgetBaseWrapper *tmpWidget = new XLuaWidgetBaseWrapper(
																	gizmo_Pool_WindowWrappers[ (int)lnV_ParentWindowID ],
																	std::string( sInput_ScriptHandler ), //handler
																	std::string( sInput_WindowCaption ), //caption
																	(int)lnV_Left, (int)lnV_Top, //left, top
																	(int)lnV_Width, //width
																	15, // ** We force the height parameters on a text box otherwise the cursor looks odd. lnV_Height, //width
																	1 //visible
																);
							tmpWidget->CreateAsTextBox();

		//attach copy and paste support!
		XPAddWidgetCallback( tmpWidget->WidgetID, gizmo_Callback_gui_WidgetHandler_CutCopyPaste);

		//add default callback function
		XPAddWidgetCallback( tmpWidget->WidgetID, gizmo_Callback_gui_WidgetHandler);

	
	

	int iWrapperPoolID = gizmo_Pool_BaseWrappers.size();
	gizmo_Pool_BaseWrappers.push_back( tmpWidget );


	lua_pushnumber(L, iWrapperPoolID);

	return 1;
}



/**
 gui.newPasswordBox( gui_window_id, "EventHandlerFunction", "Button Caption", left, top, width )
 Returns: gui_widget_id
 */
int gizmo_builtin_gui_newPasswordBox(lua_State *L){

	int n = lua_gettop(L);
	if( n < 6 ){ 
		XPLMDebugString("Gizmo: ERROR: Call to gui.newPasswordBox() failed, not enough parameters.\n");
		return 0; 
	} //MUST have 6 args, or ignored.

	



	size_t szLen;
	lua_Number lnV_ParentWindowID = lua_tonumber( L, -6 );

	const char *sInput_ScriptHandler = lua_tolstring( L, -5, &szLen );
	const char *sInput_WindowCaption = lua_tolstring( L, -4, &szLen );

	lua_Number lnV_Left = lua_tonumber( L, -3 );
	lua_Number lnV_Top = lua_tonumber( L, -2 );
	lua_Number lnV_Width = lua_tonumber( L, -1 );
	//lua_Number lnV_Height = lua_tonumber( L, -1 );


	char caDbg[2048];
	sprintf(caDbg, "Gizmo: gui.newPasswordBox( %i, %s, %s )\n", (int)lnV_ParentWindowID, sInput_ScriptHandler, sInput_WindowCaption);
	//XPLMDebugString(caDbg);



	XLuaWidgetBaseWrapper *tmpWidget = new XLuaWidgetBaseWrapper(
																	gizmo_Pool_WindowWrappers[ (int)lnV_ParentWindowID ],
																	std::string( sInput_ScriptHandler ), //handler
																	std::string( sInput_WindowCaption ), //caption
																	(int)lnV_Left, (int)lnV_Top, //left, top
																	(int)lnV_Width, //width
																	15, // ** We force the height parameters on a text box otherwise the cursor looks odd. lnV_Height, //width
																	1 //visible
																);
							tmpWidget->CreateAsPasswordBox();

		//attach copy and paste support!
		XPAddWidgetCallback( tmpWidget->WidgetID, gizmo_Callback_gui_WidgetHandler_PasswordPaste);

		//add default callback function
		XPAddWidgetCallback( tmpWidget->WidgetID, gizmo_Callback_gui_WidgetHandler);

	
	

	int iWrapperPoolID = gizmo_Pool_BaseWrappers.size();
	gizmo_Pool_BaseWrappers.push_back( tmpWidget );


	lua_pushnumber(L, iWrapperPoolID);

	return 1;
}




/**
 gui.newWindow( "WindowIdentifier" )
 Returns: gui_window_id

 Gizmo will look for and automatically call WindowIdentifier_OnCreate() to allow defered and somewhat neater code
 for creating the actual window layout.
 
 */
int gizmo_builtin_gui_newWindow(lua_State *L){

	size_t szLen;
	const char *sInput_WindowIdentifier = lua_tolstring( L, -1, &szLen );

	//char caDbg[2048];
	//sprintf(caDbg, "Gizmo: gui.newWindow( %s )\n", sInput_WindowIdentifier);
	//XPLMDebugString(caDbg);


	//Plugins > Gizmo > Windows   Menu should be re-enabled here. Make it work properly.
	XPLMEnableMenuItem(gizmo_Menu_RootItem, 4, 1);


	XLuaWidgetWindowWrapper *tmpWindow = new XLuaWidgetWindowWrapper( std::string( sInput_WindowIdentifier ) );


		XPSetWidgetProperty( tmpWindow->WidgetID, xpProperty_MainWindowHasCloseBoxes, 1);

		//add default callback function that handles window close requests
		XPAddWidgetCallback( tmpWindow->WidgetID, gizmo_Callback_gui_WindowHandler);



	int iWrapperPoolID = gizmo_Pool_WindowWrappers.size();
	gizmo_Pool_WindowWrappers.push_back( tmpWindow );




	lua_pushnumber(L, iWrapperPoolID);

	return 1;
}









