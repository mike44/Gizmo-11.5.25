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




int gizmo_builtin_gui_blur(lua_State *L);

int gizmo_builtin_gui_newButton(lua_State *L);
int gizmo_builtin_gui_newCheckbox(lua_State *L);
int gizmo_builtin_gui_newCustomWidget(lua_State *L);
int gizmo_builtin_gui_newLabel(lua_State *L);
int gizmo_builtin_gui_newListView(lua_State *L);
int gizmo_builtin_gui_newPasswordBox(lua_State *L);
int gizmo_builtin_gui_newRadioButton(lua_State *L);
int gizmo_builtin_gui_newScreen(lua_State *L);
int gizmo_builtin_gui_newSubWindow(lua_State *L);
int gizmo_builtin_gui_newTextBox(lua_State *L);
int gizmo_builtin_gui_newWindow(lua_State *L);


int gizmo_builtin_gui_getCustomWidgetPosition(lua_State *L); //returns last active custom widget size.
int gizmo_builtin_gui_getCustomWidgetSize(lua_State *L); //returns last active custom widget size.
int gizmo_builtin_gui_getCustomWidgetMouseXY(lua_State *L); //returns custom-widget relative mouse coordinates for easier drawing.
int gizmo_builtin_gui_getWidgetValue(lua_State *L);

int gizmo_builtin_gui_setWidgetValue(lua_State *L);

int gizmo_builtin_gui_hideWidget(lua_State *L);
int gizmo_builtin_gui_hideWindow(lua_State *L);

int gizmo_builtin_gui_getWindowList(lua_State *L);

int gizmo_builtin_gui_setWindowCaption(lua_State *L);
int gizmo_builtin_gui_setWindowSize(lua_State *L);

int gizmo_builtin_gui_showWidget(lua_State *L);
int gizmo_builtin_gui_showWindow(lua_State *L);

int gizmo_builtin_gui_setPasswordMode(lua_State *L);



