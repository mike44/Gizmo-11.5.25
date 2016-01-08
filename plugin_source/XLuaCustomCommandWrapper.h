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


#ifndef __GIZMO_CUSTOM_COMMAND_WRAPPER__
#define __GIZMO_CUSTOM_COMMAND_WRAPPER__

#if APL
    #include <Carbon/Carbon.h>
#endif

//handy stuff
#include "X_STL.h"
#include "XDataRefCustom.h"

//X-Plane SDK
#include "XPLMMenus.h"
#include "XPLMDisplay.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include "XPLMProcessing.h"
#include "XPLMPlugin.h"
#include "XPLMPlanes.h"
#include "XPLMGraphics.h"
#include "XPLMScenery.h"

#include "XPWidgets.h"
#include "XPStandardWidgets.h"
#include "XPUIGraphics.h"
#include "XPWidgetDefs.h"
#include "XPWidgetUtils.h"


class XLuaCustomCommandWrapper{
	public:
		XLuaCustomCommandWrapper( std::string CommandName, std::string Description, std::string ScriptHandler ){

			this->CommandName = CommandName;
			this->Description = Description;
			this->ScriptHandler = ScriptHandler;

			this->CommandID = XPLMCreateCommand( this->CommandName.c_str(), this->Description.c_str() );

		};


		std::string CommandName;
		std::string Description;
		std::string ScriptHandler;


		XPLMCommandRef CommandID;



};



#endif

