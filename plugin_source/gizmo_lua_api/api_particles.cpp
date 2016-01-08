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



// mutex code in here is rubbish - gets slow quick when you try and do fun things.
// need a message queue. fifo. queue all state changes from the users scripts and pump them
// in during a bigger step with more work done during the thread exchange.


/**
 particles.newParticles()
 Returns: particles_id - handle for particle engine, useful in other particles.api functions.
 */
int gizmo_builtin_particles_newParticles(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		XParticleEngine* particles = new XParticleEngine();
		gizmo_ParticleEngines.push_back( particles );
	
		int iParticleEngineId = gizmo_ParticleEngines.size()-1;
	
		lua_pushinteger( L, iParticleEngineId );
	
		{
			char dbg[1024]; sprintf(dbg, "Gizmo: New particle engine ID: %i\n", iParticleEngineId);
			XPLMDebugString(dbg);
		}
	
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 1;
}



/**
 particles.setBirthRate( particle_emitter_id, particles_per_second )
 */
int gizmo_builtin_particles_setBirthRate(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -2 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setBirthRate( lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}



/**
 particles.setBounce( particle_emitter_id, int_flag )
 TODO: Not sure exactly what, if any, effect particles.setBounce() has.
 */
int gizmo_builtin_particles_setBounce(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -2 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setBounce( lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}


/**
 particles.setCount( particle_emitter_id, max_particle_count )
 */
int gizmo_builtin_particles_setCount(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -2 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setCount( lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}


/**
 particles.setColor( particle_emitter_id, r,g,b, a )
 Controls the color that the next, and all subsequent, particles will be - until this is called again.
 */
int gizmo_builtin_particles_setColor(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -5 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setColor( lua_tonumber( L, -4 ), lua_tonumber( L, -3 ), lua_tonumber( L, -2 ), lua_tonumber( L, -1 ) ) ;
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}


/**
 particles.setDrag( particle_emitter_id, particle_drag )
 Alter the physics parameters.
 */
int gizmo_builtin_particles_setDrag(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -2 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setDrag( lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}



/**
 particles.setDisabled( particle_emitter_id )
 */
int gizmo_builtin_particles_setDisabled(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -1 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setEnabled( 0 );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}

/**
 particles.setEnabled( particle_emitter_id )
 */
int gizmo_builtin_particles_setEnabled(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -1 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setEnabled( 1 );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}


/**
 particles.setEnergyMax( particle_emitter_id, emax )
 */
int gizmo_builtin_particles_setEnergyMax(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }	
		int pid = lua_tointeger( L, -2 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setEnergyMax( lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}


/**
 particles.setEnergyMin( particle_emitter_id, energy_min_value )
 Controls the minimum possible value for new particles energy values.
 Energy values are randomized to give the illusion of life.
 */
int gizmo_builtin_particles_setEnergyMin(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -2 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setEnergyMin( lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}


/**
 particles.setGravity( particle_emitter_id, float_value )
 Control the strength of gravity. -9.8 would be standard earth gravity.
 Force is applied in GL Y axis. TODO: Align particles gravity to planet.
 */
int gizmo_builtin_particles_setGravity(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -2 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setGravity( lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}



/**
 particles.setHardDeck( particle_emitter_id, deck_altitude )
 */
int gizmo_builtin_particles_setHardDeck(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
	int pid = lua_tointeger( L, -2 );
	if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
	gizmo_ParticleEngines[pid]->setHardDeck( lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}



/**
 particles.setLife( particle_emitter_id, life_span_in_seconds )
 Controls how long each particle will live for.
 It is possible to "run out" of particles.
 You have a finite number and you MUST balance birth-rate:life:count variables.
 */
int gizmo_builtin_particles_setLife(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -2 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setLife( lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}



/**
 particles.setOrigin( particle_emitter_id, x,y,z )
 */
int gizmo_builtin_particles_setOrigin(lua_State *L){

// This is a speed critical function, it is very likely to be called once per frame.


	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
	
		int pid = lua_tointeger( L, -4 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		
		//speed
		gizmo_ParticleEngines[pid]->setOrigin( lua_tonumber( L, -3 ), lua_tonumber( L, -2 ), lua_tonumber( L, -1 ) );
		
		/* -- debug --
		float x = lua_tonumber( L, -3 );
		float y = lua_tonumber( L, -2 );
		float z = lua_tonumber( L, -1 );
		if(1){
			char dbg[2048];
			sprintf(dbg, "Gizmo: particles.setZeroPointForNew( %i, %.3f, %.3f, %.3f );\n",
												pid, x, y, z );
			XPLMDebugString( dbg );
		}
		gizmo_ParticleEngines[pid]->setZeroPointForNew( x, y, z );
		*/


	pthread_mutex_unlock( &gizmo_Partices_Mutex );

	return 0;
}


/**
 particles.setOriginOffset( particle_emitter_id, off_x,off_y,off_z )
 */
int gizmo_builtin_particles_setOrigin_Offset(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -4 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setOrigin_Offset( lua_tonumber( L, -3 ),lua_tonumber( L, -2 ),lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}




/**
 particles.setScaleRate( particle_emitter_id, rate_value )
 Create a new particle emitter that has a birth-size of 1.0
 Set the scale-rate to 2.0 using this function.
 When the particle engine runs, the particles will start with a size of 1.0 and when one-second has passed, they will have grown to a total size of 3.0
 */
int gizmo_builtin_particles_setScaleRate(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -2 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setGrowthRate( lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}



/**
 particles.setSize( particle_emitter_id, size )
 Control how big new particles are when they spawn.
 */
int gizmo_builtin_particles_setSize(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -2 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setSize( lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}






/**
 particles.setStuckToAcf( particle_emitter_id, sticky )
 */
int gizmo_builtin_particles_setStuckToAcf(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -2 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setStuckToAcf( lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}



/**
 particles.setType_Cubic( particle_emitter_id )
 */
int gizmo_builtin_particles_setType_Cubic(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -1 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setType_Cubic();
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}

/**
 particles.setType_Spherical( particle_emitter_id )
 */
int gizmo_builtin_particles_setType_Spherical(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -1 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setType_Spherical();
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}

/**
 particles.setType_Stream( particle_emitter_id )
 */
int gizmo_builtin_particles_setType_Stream(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -1 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setType_Stream();
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}



/**
 particles.setStreamPitch( particle_emitter_id, value )
 */
int gizmo_builtin_particles_setStreamPitch(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -2 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setStreamPitch( lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}


/**
 particles.setStreamHeading( particle_emitter_id, value )
 */
int gizmo_builtin_particles_setStreamHeading(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -2 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setStreamHeading( lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}


/**
 particles.setStreamNoise( particle_emitter_id, value )
 */
int gizmo_builtin_particles_setStreamNoise(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -2 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setStreamNoise( lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}



/**
 particles.setTexture( particle_emitter_id, texture_id )
 */
int gizmo_builtin_particles_setTexture(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -2 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->setTexture( (int)lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}


/**
 particles.setTextureSpriteCount( particle_emitter_id, nx,ny )
 Set number of sprites in the texture, eg: 3x3 = 9. texture will be sub divided into grid regions.
 FIXME: gitbug:65 particles.setTextureSpriteCount() Doesnt actually effect the output result yet.
 */
int gizmo_builtin_particles_setTextureSpriteCount(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -2 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		//gizmo_ParticleEngines[pid]->setTextureSpriteCount( lua_tonumber( L, -1 ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}
 //set number of sprites in the texture, eg: 3x3 = 9. texture will be sub divided into grid regions.




/**
 particles.start( particle_emitter_id )
 ........ redundant, we already have disable flag.
 */
int gizmo_builtin_particles_start(lua_State *L){
	while( pthread_mutex_trylock( &gizmo_Partices_Mutex ) != 0 ){ /* wait */ }
		int pid = lua_tointeger( L, -1 );
		if( pid < 0 || (unsigned int)pid >= gizmo_ParticleEngines.size() ){ pthread_mutex_unlock( &gizmo_Partices_Mutex );return 0; }
		gizmo_ParticleEngines[pid]->start( XPLMGetDataf( gizmo_xpdr_FpsMultiplier ) );
	pthread_mutex_unlock( &gizmo_Partices_Mutex );
	return 0;
}



