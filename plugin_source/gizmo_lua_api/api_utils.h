/*
 *  api_utils.h
 *  Gizmo
 *
 *  Created by Ben Russell on 22/03/11.
 *  Copyright 2011 X-Plugins.com. All rights reserved.
 *
 */




inline float gizmo_expScale( float fInput, float fScale );

int gizmo_builtin_utils_expScale(lua_State *L);
int gizmo_builtin_utils_getStringFromCPointer(lua_State *L);
int gizmo_builtin_utils_popWebBrowser(lua_State *L);

