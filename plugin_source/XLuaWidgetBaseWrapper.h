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


#ifndef	__GIZMO_WIDGET_BASE_WRAPPER__
#define __GIZMO_WIDGET_BASE_WRAPPER__


#include "XPWidgets.h"
#include "XPStandardWidgets.h"
#include "XPUIGraphics.h"
#include "XPWidgetDefs.h"
#include "XPWidgetUtils.h"

#include "X_STL.h"



#include "XLuaWidgetWindowWrapper.h"


//defaults to a button..
class XLuaWidgetBaseWrapper{
public:
	
	
	
	XPWidgetID	WidgetID;
	//XPWidgetID	ParentWidgetID;
	
	XPWidgetClass XPlaneWidgetType;
	
	//pointer handle to our parent wrapper class, allows easy access to coords
	XLuaWidgetWindowWrapper *ParentWindow;
	
	std::string ScriptHandler;
	std::string WindowCaption;
	
	int Top,Left,Width,Height;
	
	int Visible;
	
	int Right, Bottom;
	
	
	
	
	
	XLuaWidgetBaseWrapper( XLuaWidgetWindowWrapper* WidgetWindowParent, std::string ScriptHandler, std::string WindowCaption, int Left, int Top, int Width, int Height, int Visible ){
		
		this->ParentWindow = WidgetWindowParent;
		
		this->ScriptHandler = ScriptHandler;
		this->WindowCaption = WindowCaption;
		
		this->Left = this->ParentWindow->Left + Left;
		this->Top = this->ParentWindow->Top - Top;

		this->Width = Width;
		this->Height = Height;
		
		this->Right = this->Left + this->Width;
		this->Bottom = this->Top - this->Height;
		
		this->Visible = Visible;

	};
	~XLuaWidgetBaseWrapper(){
		XPDestroyWidget(this->WidgetID, 1);
	};




	void CreateAsButton(){
			this->WidgetID = XPCreateWidget(
										this->Left, //left
										this->Top, //top
										this->Right, //right
										this->Bottom, //bottom
										
										this->Visible,//visible
										this->WindowCaption.c_str(), //descriptor
										
										0,//is root widget
										this->ParentWindow->WidgetID, //inContainer - 0 for root always
										xpWidgetClass_Button //this was our bug!
										);
			this->XPlaneWidgetType = xpWidgetClass_Button;
	};
	
	void CreateAsCheckbox(){
			this->WidgetID = XPCreateWidget(
										this->Left, //left
										this->Top, //top
										this->Right, //right
										this->Bottom, //bottom
										
										this->Visible,//visible
										this->WindowCaption.c_str(), //descriptor
										
										0,//is root widget
										this->ParentWindow->WidgetID, //inContainer - 0 for root always
										xpWidgetClass_Button //this was our bug!
										);
			this->XPlaneWidgetType = xpWidgetClass_Button;

			XPSetWidgetProperty( this->WidgetID, xpProperty_ButtonType,		xpRadioButton);
			XPSetWidgetProperty( this->WidgetID, xpProperty_ButtonBehavior, xpButtonBehaviorCheckBox);
	};


	void CreateAsRadioButton(){
			this->WidgetID = XPCreateWidget(
										this->Left, //left
										this->Top, //top
										this->Right, //right
										this->Bottom, //bottom
										
										this->Visible,//visible
										this->WindowCaption.c_str(), //descriptor
										
										0,//is root widget
										this->ParentWindow->WidgetID, //inContainer - 0 for root always
										xpWidgetClass_Button //this was our bug!
										);
			this->XPlaneWidgetType = xpWidgetClass_Button;
			
			XPSetWidgetProperty( this->WidgetID, xpProperty_ButtonType,		xpRadioButton);
			XPSetWidgetProperty( this->WidgetID, xpProperty_ButtonBehavior, xpButtonBehaviorRadioButton);
	};



	void CreateAsLabel(){
			this->WidgetID = XPCreateWidget(
										this->Left, //left
										this->Top, //top
										this->Right, //right
										this->Bottom, //bottom
										
										this->Visible,//visible
										this->WindowCaption.c_str(), //descriptor
										
										0,//is root widget
										this->ParentWindow->WidgetID, //inContainer - 0 for root always
										xpWidgetClass_Caption //this was our bug!
										);
			this->XPlaneWidgetType = xpWidgetClass_Caption;
	};
	
	void CreateAsListView(){
			this->WidgetID = XPCreateWidget(
										this->Left, //left
										this->Top, //top
										this->Right, //right
										this->Bottom, //bottom
										
										this->Visible,//visible
										this->WindowCaption.c_str(), //descriptor
										
										0,//is root widget
										this->ParentWindow->WidgetID, //inContainer - 0 for root always
										xpWidgetClass_SubWindow //this was our bug!
										);
			this->XPlaneWidgetType = xpWidgetClass_SubWindow;
			XPSetWidgetProperty( this->WidgetID, xpProperty_SubWindowType, xpSubWindowStyle_ListView);
	};
	
	void CreateAsPasswordBox(){
			this->WidgetID = XPCreateWidget(
										this->Left, //left
										this->Top, //top
										this->Right, //right
										this->Bottom, //bottom
										
										this->Visible,//visible
										this->WindowCaption.c_str(), //descriptor
										
										0,//is root widget
										this->ParentWindow->WidgetID, //inContainer - 0 for root always
										xpWidgetClass_TextField //this was our bug!
										);
			this->XPlaneWidgetType = xpWidgetClass_TextField;
			XPSetWidgetProperty( this->WidgetID, xpProperty_PasswordMode, 1);
	};


	void CreateAsSubWindow(){
			this->WidgetID = XPCreateWidget(
										this->Left, //left
										this->Top, //top
										this->Right, //right
										this->Bottom, //bottom
										
										this->Visible,//visible
										this->WindowCaption.c_str(), //descriptor
										
										0,//is root widget
										this->ParentWindow->WidgetID, //inContainer - 0 for root always
										xpWidgetClass_SubWindow //this was our bug!
										);
			this->XPlaneWidgetType = xpWidgetClass_SubWindow;
			XPSetWidgetProperty( this->WidgetID, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);
	};


	void CreateAsScreen(){
			this->WidgetID = XPCreateWidget(
										this->Left, //left
										this->Top, //top
										this->Right, //right
										this->Bottom, //bottom
										
										this->Visible,//visible
										this->WindowCaption.c_str(), //descriptor
										
										0,//is root widget
										this->ParentWindow->WidgetID, //inContainer - 0 for root always
										xpWidgetClass_SubWindow //this was our bug!
										);
			this->XPlaneWidgetType = xpWidgetClass_SubWindow;
			XPSetWidgetProperty( this->WidgetID, xpProperty_SubWindowType, xpSubWindowStyle_Screen);
	};


	
	void CreateAsTextBox(){
			this->WidgetID = XPCreateWidget(
										this->Left, //left
										this->Top, //top
										this->Right, //right
										this->Bottom, //bottom
										
										this->Visible,//visible
										this->WindowCaption.c_str(), //descriptor
										
										0,//is root widget
										this->ParentWindow->WidgetID, //inContainer - 0 for root always
										xpWidgetClass_TextField //this was our bug!
										);
			this->XPlaneWidgetType = xpWidgetClass_TextField;
	};


	
};





#endif
