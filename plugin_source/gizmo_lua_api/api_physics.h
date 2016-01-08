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




int gizmo_builtin_physics_setPhysicsDrawingHack(lua_State *L);
int gizmo_builtin_physics_setEnabled(lua_State *L);
int gizmo_builtin_physics_setPaused(lua_State *L);
int gizmo_builtin_physics_stepOnce(lua_State *L);


int gizmo_builtin_physics_newSphere( lua_State *L );
int gizmo_builtin_physics_newBox( lua_State *L );
int gizmo_builtin_physics_newCylinder( lua_State *L );//cyl
int gizmo_builtin_physics_newCapsule( lua_State *L );
int gizmo_builtin_physics_newCone( lua_State *L );//cone
int gizmo_builtin_physics_newMultiSphere( lua_State *L );//m-sphere


//no hooks into bullet for these.
// convex hull
// convex tri mesh
// bvh tri mesh
// height field terrain
// plane




int gizmo_builtin_physics_loadCollada( lua_State *L );


int gizmo_builtin_physics_saveCollada( lua_State *L );



//rigid body control functions - forces etc.
int gizmo_builtin_physics_rb_applyForce( lua_State *L ); //apply central force
int gizmo_builtin_physics_rb_applyImpulse( lua_State *L ); //apply central impulse
int gizmo_builtin_physics_rb_applyTorqueImpulse( lua_State *L ); //apply torque impulse
int gizmo_builtin_physics_rb_applyOffsetImpulse( lua_State *L ); //apply impulse at location
int gizmo_builtin_physics_rb_applyOffsetForce( lua_State *L ); //apply force at location
int gizmo_builtin_physics_rb_applyTorque( lua_State *L ); //apply torque

int gizmo_builtin_physics_rb_clearForces( lua_State *L ); //clear forces

int gizmo_builtin_physics_rb_getOrientationQ( lua_State *L ); //getOrientationQ

int gizmo_builtin_physics_rb_getLinearVelocity( lua_State *L ); //getLinearVelocity
int gizmo_builtin_physics_rb_getAngularVelocity( lua_State *L ); //getAngularVelocity

int gizmo_builtin_physics_rb_setLinearVelocity( lua_State *L ); //setLinearVelocity
int gizmo_builtin_physics_rb_setAngularVelocity( lua_State *L ); //setAngularVelocity

int gizmo_builtin_physics_rb_translate( lua_State *L ); //translate





int gizmo_builtin_physics_restart(lua_State *L);
int gizmo_builtin_physics_setBrush( lua_State *L );
int gizmo_builtin_physics_setBrushByID( lua_State *L );


int gizmo_builtin_physics_setCursor( lua_State *L );


int gizmo_builtin_physics_setDebugMode( lua_State *L );
int gizmo_builtin_physics_setGravityVector( lua_State *L );
int gizmo_builtin_physics_setInertia( lua_State *L );
int gizmo_builtin_physics_setMass( lua_State *L );
int gizmo_builtin_physics_setOrigin( lua_State *L );






