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


#ifndef	__GIZMO_WIDGET_WINDOW_WRAPPER__
#define __GIZMO_WIDGET_WINDOW_WRAPPER__


#include "XPWidgets.h"
#include "XPStandardWidgets.h"
#include "XPUIGraphics.h"
#include "XPWidgetDefs.h"
#include "XPWidgetUtils.h"

#include "X_STL.h"




class XLuaWidgetWindowWrapper{
public:

	XLuaWidgetWindowWrapper( std::string WindowIdentifier ){//, std::string WindowCaption, int Left, int Top, int Width, int Height, int Visible ){
		this->WindowIdentifier = WindowIdentifier;

		this->WindowCaption = "Gizmo - New Window";
		/*
		//user is expected to set this in their window_id_OnCreate() function
		
		this->Left = Left;
		this->Top = Top;

		this->Width = Width;
		this->Height = Height;

		this->Right = this->Left + this->Width;
		this->Bottom = this->Top - this->Height;
		*/

		this->Visible = 0;

		//create a temporary small invisible window and resize it from the make_window.txt script, neater packaging.
		this->WidgetID = XPCreateWidget(
										300, //this->Left, //left
										600, //this->Top, //top
										700, //this->Right, //right
										300, //this->Bottom, //bottom

										this->Visible,//visible
										this->WindowCaption.c_str(), //descriptor

										1,//is root widget
										0, //inContainer - 0 for root always
										xpWidgetClass_MainWindow //this was our bug!
										);

		this->NeedInit = 1; //this sets up our defered calls to fill in the widgets

		
	};
	~XLuaWidgetWindowWrapper(){
		XPDestroyWidget(this->WidgetID, 1);
	};


	void Show(){
		this->Visible = 1;
		XPShowWidget( this->WidgetID );
		XPBringRootWidgetToFront( this->WidgetID );

	};

	void Hide(){
		this->Visible = 0;
		XPHideWidget( this->WidgetID );

	};



	XPWidgetID	WidgetID;

	std::string WindowIdentifier;
	std::string WindowCaption;
	
	int Top,Left,Width,Height;

	int Visible;

	int Right, Bottom;

	int NeedInit;

protected:

private:

};





#endif
