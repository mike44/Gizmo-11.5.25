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



#define DEBUG_PHYSICS_API_CALL 1



/*
 physics.api
 Control the Bullet Physics systems.
 */



/**
 physics.applyForce( object_id, x,y,z )
 Apply a force to the center of the object, force is specified as a vector.
 */
int gizmo_builtin_physics_rb_applyForce( lua_State *L ){

	if( lua_gettop( L ) < 4 ){
		XPLMDebugString("Gizmo: physics.applyForce(..), not enough arguments, expected 4.\n");
		return 0;
	} //not enough args, abort.



	lua_Integer objectId = lua_tointeger( L, -4 );

	lua_Number fx = lua_tonumber( L, -3 );
	lua_Number fy = lua_tonumber( L, -2 );
	lua_Number fz = lua_tonumber( L, -1 );


		#if DEBUG_PHYSICS_API_CALL
		char caDbg[512];
		sprintf( caDbg, "Gizmo: physics.applyForce( %i,  %0.5f, %0.5f, %0.5f )\n",  (int)objectId,  (float)fx, (float)fy, (float)fz  );
		XPLMDebugString( caDbg );
		#endif


	gizmo_BulletPhysics->rb_applyCentralForce( objectId,  fx, fy, fz );


	return 0;

}; //apply central force




/**
 physics.applyImpulse( object_id, x,y,z )
 Apply an impulse to the center of the object, impulse is specified as a vector.
 */
int gizmo_builtin_physics_rb_applyImpulse( lua_State *L ){

	if( lua_gettop( L ) < 4 ){
		XPLMDebugString("Gizmo: physics.applyImpulse(..), not enough arguments, expected 4.\n");
		return 0;
	} //not enough args, abort.



	lua_Integer objectId = lua_tointeger( L, -4 );

	lua_Number fx = lua_tonumber( L, -3 );
	lua_Number fy = lua_tonumber( L, -2 );
	lua_Number fz = lua_tonumber( L, -1 );


		#if DEBUG_PHYSICS_API_CALL
		char caDbg[512];
		sprintf( caDbg, "Gizmo: physics.applyImpulse( %i,  %0.5f, %0.5f, %0.5f )\n",  (int)objectId,  (float)fx, (float)fy, (float)fz  );
		XPLMDebugString( caDbg );
		#endif


	gizmo_BulletPhysics->rb_applyCentralImpulse( objectId,  fx, fy, fz );


	return 0;

}; //apply central impulse





/**
 physics.applyTorqueImpulse( object_id, x,y,z )
 Apply a torque impulse to the center of the object, torque impulse is specified as a vector.
 */
int gizmo_builtin_physics_rb_applyTorqueImpulse( lua_State *L ){

	if( lua_gettop( L ) < 4 ){
		XPLMDebugString("Gizmo: physics.applyTorqueImpulse(..), not enough arguments, expected 4.\n");
		return 0;
	} //not enough args, abort.



	lua_Integer objectId = lua_tointeger( L, -4 );

	lua_Number fx = lua_tonumber( L, -3 );
	lua_Number fy = lua_tonumber( L, -2 );
	lua_Number fz = lua_tonumber( L, -1 );


		#if DEBUG_PHYSICS_API_CALL
		char caDbg[512];
		sprintf( caDbg, "Gizmo: physics.applyTorqueImpulse( %i,  %0.5f, %0.5f, %0.5f )\n",  (int)objectId,  (float)fx, (float)fy, (float)fz  );
		XPLMDebugString( caDbg );
		#endif


	gizmo_BulletPhysics->rb_applyTorqueImpulse( objectId,  fx, fy, fz );


	return 0;

}; //apply torque impulse



/**
 physics.applyOffsetImpulse( object_id, x,y,z ) - FIXME: empty hook
 Apply a  to the center of the object,  is specified as a vector.
 */
int gizmo_builtin_physics_rb_applyOffsetImpulse( lua_State *L ){
	return 0;
}; //apply impulse at location



/**
 physics.applyTorqueImpulse( object_id, x,y,z ) - FIXME: empty hook
 Apply a  to the center of the object,  is specified as a vector.
 */
int gizmo_builtin_physics_rb_applyOffsetForce( lua_State *L ){
	return 0;
}; //apply force at location



/**
 physics.applyTorque( object_id, x,y,z )
 Apply torque the center of the object, torque is specified as a vector.
 */
int gizmo_builtin_physics_rb_applyTorque( lua_State *L ){

	if( lua_gettop( L ) < 4 ){
		XPLMDebugString("Gizmo: physics.applyTorque(..), not enough arguments, expected 4.\n");
		return 0;
	} //not enough args, abort.


	lua_Integer objectId = lua_tointeger( L, -4 );

	lua_Number fx = lua_tonumber( L, -3 );
	lua_Number fy = lua_tonumber( L, -2 );
	lua_Number fz = lua_tonumber( L, -1 );


		#if DEBUG_PHYSICS_API_CALL
		char caDbg[512];
		sprintf( caDbg, "Gizmo: physics.applyTorque( %i,  %0.5f, %0.5f, %0.5f )\n",  (int)objectId,  (float)fx, (float)fy, (float)fz  );
		XPLMDebugString( caDbg );
		#endif


	gizmo_BulletPhysics->rb_applyTorque( objectId,  fx, fy, fz );


	return 0;

}; //apply torque




/**
 physics.clearForces( object_id )
 Clears any force parameters for the object.
 */
int gizmo_builtin_physics_rb_clearForces( lua_State *L ){

	if( lua_gettop( L ) < 1 ){
		XPLMDebugString("Gizmo: physics.clearForces(..), not enough arguments, expected 1.\n");
		return 0;
	} //not enough args, abort.


	lua_Integer objectId = lua_tointeger( L, -1 );


		#if DEBUG_PHYSICS_API_CALL
		char caDbg[512];
		sprintf( caDbg, "Gizmo: physics.clearForces( %i )\n",  (int)objectId  );
		XPLMDebugString( caDbg );
		#endif


	gizmo_BulletPhysics->rb_clearForces( objectId );


	return 0;

}; //clear forces



/**
 physics.getAngularVelocity( object_id )
 
 Returns: x,y,z
 
 API: >= 11.5.25
 */
int gizmo_builtin_physics_rb_getAngularVelocity( lua_State *L ){
	
	float fRetVector[3] = {0.f, 0.f, 0.f};
	
	if( lua_gettop( L ) < 1 ){
		XPLMDebugString("Gizmo: physics.getAngularVelocity(..), not enough arguments, expected 1.\n");
		return 0;
	} //not enough args, abort.
	
	
	lua_Integer objectId = lua_tointeger( L, -1 );
	
	gizmo_BulletPhysics->rb_getAngularVelocity( objectId, fRetVector );
	
		lua_pushnumber(L, fRetVector[0]);
		lua_pushnumber(L, fRetVector[1]);
		lua_pushnumber(L, fRetVector[2]);
	
	
	return 3;
	
}; //getAngularVelocity





/**
 physics.getLinearVelocity( object_id )
 
 Returns: x,y,z
 
 API: >= 11.5.25
 */
int gizmo_builtin_physics_rb_getLinearVelocity( lua_State *L ){

	float fRetVector[3] = {0.f, 0.f, 0.f};
	
	if( lua_gettop( L ) < 1 ){
		XPLMDebugString("Gizmo: physics.getLinearVelocity(..), not enough arguments, expected 1.\n");
		return 0;
	} //not enough args, abort.
	
	
	lua_Integer objectId = lua_tointeger( L, -1 );
	
	gizmo_BulletPhysics->rb_getLinearVelocity( objectId, fRetVector );
	
	lua_pushnumber(L, fRetVector[0]);
	lua_pushnumber(L, fRetVector[1]);
	lua_pushnumber(L, fRetVector[2]);
	
	
	return 3;

}; //getLinearVelocity




/**
 physics.getOrientationQ( object_id )
 
 Returns: x,y,z,angle
 
 These values represent the attitude Quarternion for the specified object_id.
 
 API: >= 11.5.25
 */
int gizmo_builtin_physics_rb_getOrientationQ( lua_State *L ){
	
	
	float fRetVector[4] = {0.f, 0.f, 0.f, 0.f};
	
	if( lua_gettop( L ) < 1 ){
		XPLMDebugString("Gizmo: physics.getOrientationQ(..), not enough arguments, expected 1.\n");
		return 0;
	} //not enough args, abort.
	
	
	lua_Integer objectId = lua_tointeger( L, -1 );
	
	gizmo_BulletPhysics->rb_getOrientationQ( objectId, fRetVector );
	
	lua_pushnumber(L, fRetVector[0]);
	lua_pushnumber(L, fRetVector[1]);
	lua_pushnumber(L, fRetVector[2]);
	lua_pushnumber(L, fRetVector[3]);
	
	
	return 4;

	
}; //getOrientationQ




/**
 physics.setLinearVelocity( object_id, vx, vy, vz )
 
 API: >= 11.5.25
 */
int gizmo_builtin_physics_rb_setLinearVelocity( lua_State *L ){
	if( lua_gettop( L ) < 4 ){
		XPLMDebugString("Gizmo: physics.setLinearVelocity(..), not enough arguments, expected 4.\n");
		return 0;
	} //not enough args, abort.
	
	lua_Integer objectId = lua_tointeger( L, -4 );
	
	lua_Number fx = lua_tonumber( L, -3 );
	lua_Number fy = lua_tonumber( L, -2 );
	lua_Number fz = lua_tonumber( L, -1 );
	
	
		#if DEBUG_PHYSICS_API_CALL
		char caDbg[512];
		sprintf( caDbg, "Gizmo: physics.setLinearVelocity( %i,  %0.5f, %0.5f, %0.5f )\n",  (int)objectId,  (float)fx, (float)fy, (float)fz  );
		XPLMDebugString( caDbg );
		#endif
	
	
	gizmo_BulletPhysics->rb_setLinearVelocity( objectId,  fx, fy, fz );
	
	
	return 0;
	
}; //setLinearVelocity



/**
physics.setAngularVelocity( id, x, y, z )
 
 Apply force to rigid body instance.
 
 */
int gizmo_builtin_physics_rb_setAngularVelocity( lua_State *L ){

	if( lua_gettop( L ) < 4 ){
		XPLMDebugString("Gizmo: physics.setAngularVelocity(..), not enough arguments, expected 4.\n");
		return 0;
	} //not enough args, abort.
	
	lua_Integer objectId = lua_tointeger( L, -4 );
	
	lua_Number fx = lua_tonumber( L, -3 );
	lua_Number fy = lua_tonumber( L, -2 );
	lua_Number fz = lua_tonumber( L, -1 );
	
	
		#if DEBUG_PHYSICS_API_CALL
		char caDbg[512];
		sprintf( caDbg, "Gizmo: physics.setAngularVelocity( %i,  %0.5f, %0.5f, %0.5f )\n",  (int)objectId,  (float)fx, (float)fy, (float)fz  );
		XPLMDebugString( caDbg );
		#endif
	
	
	gizmo_BulletPhysics->rb_setAngularVelocity( objectId,  fx, fy, fz );
	
	
	return 0;
}; //setAngularVelocity





/**
 physics.translate( id, x,y,z )
 
 Translate a rigid body inside the physics world.
 
 API: >= 11.5.25
 */
int gizmo_builtin_physics_rb_translate( lua_State *L ){
	if( lua_gettop( L ) < 4 ){
		XPLMDebugString("Gizmo: physics.translate(..), not enough arguments, expected 4.\n");
		return 0;
	} //not enough args, abort.
	
	lua_Integer objectId = lua_tointeger( L, -4 );
	
	lua_Number fx = lua_tonumber( L, -3 );
	lua_Number fy = lua_tonumber( L, -2 );
	lua_Number fz = lua_tonumber( L, -1 );
	
	
		#if DEBUG_PHYSICS_API_CALL
		char caDbg[512];
		sprintf( caDbg, "Gizmo: physics.translate( %i,  %0.5f, %0.5f, %0.5f )\n",  (int)objectId,  (float)fx, (float)fy, (float)fz  );
		XPLMDebugString( caDbg );
		#endif
	
	
	gizmo_BulletPhysics->rb_translate( objectId,  fx, fy, fz );
	
	
	return 0;
}; //translate





/**
 physics.restart()
 
 Reset the physics engine.
 */
int gizmo_builtin_physics_restart(lua_State *L){

	XPLMDebugString("Gizmo: Lua script restarting bullet physics.\n");

		gizmo_StopBulletPhysics();
		gizmo_StartBulletPhysics();

	lua_pushnumber(L, 1);

	return 1;
}



/**
 physics.setMass( new_default_mass )
 
 Set the default mass value for new objects, ratio.
 */
int gizmo_builtin_physics_setMass(lua_State *L){

	lua_Number lnV_Mass = lua_tonumber( L, -1 );

		char caDbg[512];
		sprintf(caDbg, "Gizmo: physics.setMass( %0.3f )\n", (float)lnV_Mass );
		XPLMDebugString( caDbg );

		gizmo_BulletPhysics->SetMass( lnV_Mass );

	return 0;
}



/**
 physics.setPhysicsDrawingHack( int_flag )
 
 Turns a physics drawing hack on and off, you MUST load an OBJ8 file (anyone will do) before turning this on.
 It's a _hack_.
 */
int gizmo_builtin_physics_setPhysicsDrawingHack(lua_State *L){

	lua_Number lnV = lua_tonumber( L, -1 );

		switch( (int)lnV ){
			default:
			case 0:
				XPLMDebugString("Gizmo: Disabled physics drawing hack.\n");
				gizmo_bUsePhysicsDrawingHack = false;
				break;

			case 1:
				XPLMDebugString("Gizmo: Enabled physics drawing hack.\n");
				gizmo_bUsePhysicsDrawingHack = true;
				break;

		}

	return 0;
}



/**
 physics.setEnabled( int_flag )
 
 Enable the physics engine.
 */
int gizmo_builtin_physics_setEnabled(lua_State *L){

	lua_Number lnV = lua_tonumber( L, -1 );

		switch( (int)lnV ){
			default:
			case 0:
				//default x-plane engine sim.
				XPLMDebugString("Gizmo: physics disabled.\n");
				gizmo_bUsePhysics = false;
				break;

			case 1:
				//enable nasa turbojet sim module
				XPLMDebugString("Gizmo: physics enabled.\n");
				gizmo_bUsePhysics = true;
				break;

		}

	return 0;
}



/**
 physics.setPaused( int_flag )
 
 Pause the physics engine.
 */
int gizmo_builtin_physics_setPaused(lua_State *L){

	lua_Number lnV = lua_tonumber( L, -1 );

		switch( (int)lnV ){
			default:
			case 0:
				//default x-plane engine sim.
				XPLMDebugString("Gizmo: physics running..\n");
				gizmo_bPhysicsPaused = false;
				break;

			case 1:
				//enable nasa turbojet sim module
				XPLMDebugString("Gizmo: physics paused.\n");
				gizmo_bPhysicsPaused = true;
				break;

		}

	return 0;
}




/**
 physics.stepOnce()
 
 When paused, step the physics engine frame by frame.
 */
int gizmo_builtin_physics_stepOnce(lua_State *L){

	lua_Number lnV = lua_tonumber( L, -1 );

		switch( (int)lnV ){
			default:
			case 0:
				//Setup a guard flag to trigger on the next iteration of the flight loop, the guard flag will trip code that steps the bullet physics engine.
				gizmo_bPhysics_StepOnce = false;
				break;

			case 1:
				XPLMDebugString("Gizmo: Physics - Stepping one frame..\n");
				gizmo_bPhysics_StepOnce = true;
				break;

		}

	return 0;
}




/**
 physics.setGravityVector( x,y,z )
 
 Set default graivty vector.
 Normal gravity is ( 0, -9.8, 0 )
 */
int gizmo_builtin_physics_setGravityVector(lua_State *L){

	lua_Number lnV_X = lua_tonumber( L, -3 );
	lua_Number lnV_Y = lua_tonumber( L, -2 );
	lua_Number lnV_Z = lua_tonumber( L, -1 );

		char caDbg[512];
		sprintf(caDbg, "Gizmo: physics.setGravityVector( %0.3f, %0.3f, %0.3f )\n", (float)lnV_X, (float)lnV_Y, (float)lnV_Z );
		XPLMDebugString( caDbg );

		gizmo_BulletPhysics->SetGravityVector( lnV_X, lnV_Y, lnV_Z );

	return 0;
}



/**
 physics.setOrigin( x,y,z )
 
 Controls where in X-Planes OpenGL world the Bullet-Origin is.
 Bullets physics math works best with a 10x10x10 kilometer cube.
 Move it around.
 */
int gizmo_builtin_physics_setOrigin(lua_State *L){

	lua_Number lnV_X = lua_tonumber( L, -3 );
	lua_Number lnV_Y = lua_tonumber( L, -2 );
	lua_Number lnV_Z = lua_tonumber( L, -1 );

		char caDbg[512];
		sprintf(caDbg, "Gizmo: physics.setOrigin( %0.3f, %0.3f, %0.3f )\n", (float)lnV_X, (float)lnV_Y, (float)lnV_Z );
		XPLMDebugString( caDbg );

			gizmo_fPhysicsWorldOffsetX = (float)lnV_X;
			gizmo_fPhysicsWorldOffsetY = (float)lnV_Y;
			gizmo_fPhysicsWorldOffsetZ = (float)lnV_Z;

	return 0;
}



/**
 physics.setInertia( vx,vy,vz )
 
 Set default inertia value for newly created objects.
 */
int gizmo_builtin_physics_setInertia(lua_State *L){

	lua_Number lnV_X = lua_tonumber( L, -3 );
	lua_Number lnV_Y = lua_tonumber( L, -2 );
	lua_Number lnV_Z = lua_tonumber( L, -1 );

		char caDbg[512];
		sprintf(caDbg, "Gizmo: physics.setInertia( %0.3f, %0.3f, %0.3f )\n", (float)lnV_X, (float)lnV_Y, (float)lnV_Z );
		XPLMDebugString( caDbg );

		gizmo_BulletPhysics->SetIntertia( lnV_X, lnV_Y, lnV_Z );

	lua_pushnumber(L, 1);

	return 1;
}



/**
 physics.setBrush( object_id )
 
 Set default brush for newly created objects.
 The brush_id controls what the physics object will look like when drawn on screen.
 It does NOT control the physics calculations for collisions.
 */
int gizmo_builtin_physics_setBrush(lua_State *L){

	lua_Number lnV_GraphicsObject = lua_tonumber( L, -1 );

		char caDbg[512];
		sprintf(caDbg, "Gizmo: physics.setBrush( %i )\n", (int)lnV_GraphicsObject );
		XPLMDebugString( caDbg );

		//gizmo_BulletPhysics->SetCursor( lnV_X, lnV_Y, lnV_Z );

		gizmo_Physics_DefaultBrushObject = (int)lnV_GraphicsObject;

	lua_pushnumber(L, 1);

	return 1;
}



/**
 physics.setBrushByID( physics_id, object_id )
 
 Change the brush-id of a physics object.
 */
int gizmo_builtin_physics_setBrushByID(lua_State *L){

	lua_Integer lnV_PhysicsObject = lua_tointeger( L, -2 );
	lua_Integer lnV_GraphicsObject = lua_tointeger( L, -1 );

	char caDbg[512];
	sprintf(caDbg, "Gizmo: physics.setBrushByID( %i, %i )\n", (int)lnV_PhysicsObject, (int)lnV_GraphicsObject );
	XPLMDebugString( caDbg );

		if( lnV_PhysicsObject >= 0 ){
			if( lnV_PhysicsObject < gizmo_svPhysicsObjectBrushes.size() ){
				gizmo_svPhysicsObjectBrushes[ lnV_PhysicsObject ] = lnV_GraphicsObject;
			}
		}else{

		}

	return 0;
}



/**
 physics.setCursor( x,y,z )
 
 Move the physics spawn point in space.
 */
int gizmo_builtin_physics_setCursor(lua_State *L){

	lua_Number lnV_X = lua_tonumber( L, -3 );
	lua_Number lnV_Y = lua_tonumber( L, -2 );
	lua_Number lnV_Z = lua_tonumber( L, -1 );

		#if 0
			char caDbg[512];
			sprintf(caDbg, "Gizmo: physics.setCursor( %0.3f, %0.3f, %0.3f )\n", (float)lnV_X, (float)lnV_Y, (float)lnV_Z );
			XPLMDebugString( caDbg );
		#endif

		gizmo_BulletPhysics->SetCursor( lnV_X, lnV_Y, lnV_Z );

	lua_pushnumber(L, 1);

	return 1;
}




/**
 physics.setDebugMode( debug_mode_flag )
 
 Possible values:
 enum  	DebugDrawModes { 
 DBG_NoDebug = 0, 
 DBG_DrawWireframe = 1, 
 DBG_DrawAabb = 2, 
 DBG_DrawFeaturesText = 4, 
 DBG_DrawContactPoints = 8, 
 DBG_NoDeactivation = 16, 
 DBG_NoHelpText = 32, 
 DBG_DrawText = 64, 
 DBG_ProfileTimings = 128, 
 DBG_EnableSatComparison = 256, 
 DBG_DisableBulletLCP = 512, 
 DBG_EnableCCD = 1024, 
 DBG_DrawConstraints = (1 << 11), 
 DBG_DrawConstraintLimits = (1 << 12), 
 DBG_FastWireframe = (1<<13), 
 DBG_MAX_DEBUG_DRAW_MODE 
 }
 */
int gizmo_builtin_physics_setDebugMode(lua_State *L){

	//This function toggles bullets debug-drawing function on and off.
	//Reference values: http://www.bulletphysics.com/Bullet/BulletFull/classbtIDebugDraw.html


		lua_Number lnV_M = lua_tonumber( L, -1 );

			char caDbg[512];
			sprintf(caDbg, "Gizmo: physics.setDebugMode( %i )\n", (int)lnV_M );
			XPLMDebugString( caDbg );

		//this->gDebugDrawer.setDebugMode( btIDebugDraw::DBG_DrawWireframe );

		gizmo_BulletPhysics->gDebugDrawer.setDebugMode( (int)lnV_M );
		
	return 0;

}



/**
 physics.newSphere( radius )
 
 Create a new sphere rigid body in the physics engine.
 */
int gizmo_builtin_physics_newSphere(lua_State *L){

		lua_Number lnV_R = lua_tonumber( L, -1 );

		#if DEBUG_PHYSICS_API_CALL
		char caDbg[512];
		sprintf(caDbg, "Gizmo: physics.newSphere( %0.3f )\n", (float)lnV_R);
		XPLMDebugString( caDbg );
		#endif

		gizmo_BulletPhysics->CreateSphere( lnV_R );
		gizmo_svPhysicsObjectBrushes.push_back(gizmo_Physics_DefaultBrushObject);

		lua_pushinteger( L, gizmo_svPhysicsObjectBrushes.size()-1 );
		
		return 1;

}


/**
 physics.newBox( edge_size )
 
 Creates a new rigid body element in the physics engine.
 
 */
int gizmo_builtin_physics_newBox(lua_State *L){

		lua_Number lnV_X = lua_tonumber( L, -3 );
		lua_Number lnV_Y = lua_tonumber( L, -2 );
		lua_Number lnV_Z = lua_tonumber( L, -1 );

		#if DEBUG_PHYSICS_API_CALL
		char caDbg[512];
		sprintf(caDbg, "Gizmo: physics.newBox( %0.3f, %0.3f, %0.3f )\n", (float)lnV_X, (float)lnV_Y, (float)lnV_Z);
		XPLMDebugString( caDbg );
		#endif

		gizmo_BulletPhysics->CreateBox( lnV_X, lnV_Y, lnV_Z );
		gizmo_svPhysicsObjectBrushes.push_back(gizmo_Physics_DefaultBrushObject);

		lua_pushinteger( L, gizmo_svPhysicsObjectBrushes.size()-1 );
		
		return 1;

}



/**
 physics.newCylinder( radius, height )
 
  Creates a new rigid body element in the physics engine.
 
 */
int gizmo_builtin_physics_newCylinder(lua_State *L){


		lua_Number lnV_X = lua_tonumber( L, -3 );
		lua_Number lnV_Y = lua_tonumber( L, -2 );
		lua_Number lnV_Z = lua_tonumber( L, -1 );

		#if DEBUG_PHYSICS_API_CALL
		char caDbg[512];
		sprintf(caDbg, "Gizmo: physics.newCylinder( %0.3f, %0.3f, %0.3f )\n", (float)lnV_X, (float)lnV_Y, (float)lnV_Z);
		XPLMDebugString( caDbg );
		#endif

		gizmo_BulletPhysics->CreateCylinder( lnV_X, lnV_Y, lnV_Z );
		gizmo_svPhysicsObjectBrushes.push_back(gizmo_Physics_DefaultBrushObject);

		lua_pushinteger( L, gizmo_svPhysicsObjectBrushes.size()-1 );

		return 1;

}




/**
 physics.newCapsule( radius, height )
 
 Creates a new rigid body element in the physics engine.
 A capsule is a cylinder with domed ends.
 
 */
int gizmo_builtin_physics_newCapsule(lua_State *L){

		lua_Number lnV_R = lua_tonumber( L, -2 );
		lua_Number lnV_H = lua_tonumber( L, -1 );

		#if DEBUG_PHYSICS_API_CALL
		char caDbg[512];
		sprintf(caDbg, "Gizmo: physics.newCapsule( %0.3f, %0.3f )\n", (float)lnV_R, (float)lnV_H );
		XPLMDebugString( caDbg );
		#endif

		gizmo_BulletPhysics->CreateCapsule( lnV_R, lnV_H );
		gizmo_svPhysicsObjectBrushes.push_back(gizmo_Physics_DefaultBrushObject);

		lua_pushinteger( L, gizmo_svPhysicsObjectBrushes.size()-1 );

		return 1;

}




/**
 physics.newCone( radius, height )
 
 Creates a new rigid body element in the physics engine.
 */
int gizmo_builtin_physics_newCone(lua_State *L){

		lua_Number lnV_R = lua_tonumber( L, -2 );
		lua_Number lnV_H = lua_tonumber( L, -1 );

		#if DEBUG_PHYSICS_API_CALL
		char caDbg[512];
		sprintf(caDbg, "Gizmo: physics.newCone( %0.3f, %0.3f )\n", (float)lnV_R, (float)lnV_H );
		XPLMDebugString( caDbg );
		#endif

		gizmo_BulletPhysics->CreateCone( lnV_R, lnV_H );
		gizmo_svPhysicsObjectBrushes.push_back(gizmo_Physics_DefaultBrushObject);

		lua_pushinteger( L, gizmo_svPhysicsObjectBrushes.size()-1 );

		return 1;

}


/**
 physics.newMultiSphere( ........fubar )
 
 Creates a new rigid body element in the physics engine.
 */
int gizmo_builtin_physics_newMultiSphere( lua_State *L ){ //FIXME: empty hook

};//m-sphere



/**
 physics.loadCollada( "filename" )
 
 NON functional.
 */
int gizmo_builtin_physics_loadCollada(lua_State *L){ //FIXME: empty hook

		//gizmo_BulletPhysics->ColladaLoad();
        XPLMSpeakString("Bullet Collada disabled.");

	return 0;
}


/* *
 physics.saveCollada( "filename" )
 
 NON functional.
 
 */
int gizmo_builtin_physics_saveCollada(lua_State *L){ //FIXME: empty hook

		//gizmo_BulletPhysics->ColladaSave();
		XPLMSpeakString("Bullet Collada disabled.");

	return 0;
}



