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
 sound.say( "string to vocalize" )
 Causes X-Plane to speak your text using the text-to-speech processor like that of the built in ATC system.
 */
int gizmo_builtin_sound_say(lua_State *L){


	if (lua_isstring(L, -1)){

		size_t szLen;
		const char *sInput = lua_tolstring( L, -1, &szLen );

			XPLMSpeakString( sInput );
			
			char caDbg[1024];
			sprintf(caDbg,"Gizmo: sound.say(%s)\n", sInput );
			XPLMDebugString( caDbg );
			

	}

	//lua_pushnumber(L, 1);

	return 0;

}



/**
 sound.newSound()
 Returns: sound_id
 Allocates a new slot and returns a handle to it.
 */
int gizmo_builtin_sound_newSound( lua_State *L ){

    lua_pushinteger( L, gizmo_OpenAL->GetSoundSlot() );

    return 1;
}



/**
 sound.load( [sound_id], "sound_filename.wav" )
 Returns: sound_id
 If called with no sound_id argument, sound.load("filename.wav") will create a new sound slot.
 * Formats supported:
 ** 22 or 44kz, 8 or 16 bit, mono or stero. Any combination.
 
 * Sounds do not load on PowerPC Macintosh. *
 */
int gizmo_builtin_sound_load( lua_State *L ){

	int iArgCount = lua_gettop( L );	
	int iSoundId = -1;

	switch( iArgCount ){
		case 1:
			{
				if (lua_isstring(L, -1)){
					
					size_t szLen;
					const char *sInput = lua_tolstring( L, -1, &szLen );
					
						iSoundId = gizmo_OpenAL->GetSoundSlot();

						gizmo_OpenAL->LoadSound( iSoundId, sInput );

						char caTmpDbg[2048];
						sprintf(caTmpDbg, "Gizmo: sound.load('%s') returns id: %i\n", sInput, iSoundId );
						XPLMDebugString( caTmpDbg );

				}
			}
			break;

	
		//user is calling sound.load( sound_id, "foo.wav" )
			//load a sound into an existing sound slot.
		case 2:
			{
				if (lua_isstring(L, -1)){
					
					size_t szLen;
					iSoundId = lua_tointeger( L, -2 );
					const char *sInput = lua_tolstring( L, -1, &szLen );
					
					gizmo_OpenAL->LoadSound( iSoundId, sInput );
					
					char caTmpDbg[2048];
					sprintf(caTmpDbg, "Gizmo: sound.load(%i, '%s') returns id: %i\n", iSoundId, sInput, iSoundId );
					XPLMDebugString( caTmpDbg );
					
				}//end if
				
			}
			break;
			
			
		default:
			{
			char caTmpDbg[2048];
			sprintf(caTmpDbg, "Gizmo: Syntax Error: sound.load(...)\n" );
			XPLMDebugString( caTmpDbg );
			}
			break;
			
			
	}//end switch( iArgCount );
	
	
	
	lua_pushnumber( L, iSoundId );
	return 1;
	
}//end sound.load()





/**
 sound.pause( sound_id )
 
 Pause the playback of the specified sound.
 */
int gizmo_builtin_sound_pause( lua_State *L ){

    //XPLMDebugString("Gizmo: sound.pause()\n");

	lua_Integer lnVIndexID = lua_tointeger( L, -1 );
	gizmo_OpenAL->Pause( (int)lnVIndexID );

    return 0;
}



/**
 sound.play( sound_id )
 
 Play the specified sound. Resumes from paused state if sound was paused.
 See sound.rewind(...)
 */
int gizmo_builtin_sound_play( lua_State *L ){

    lua_Integer lnVIndexID = lua_tointeger( L, -1 );
	gizmo_OpenAL->Play( (int)lnVIndexID );

    //XPLMDebugString("Gizmo: sound.play()\n");

    return 0;
}



/**
 sound.rewind( sound_id )
 
 Move the playhead for a given sample back to the start of a sample.
 */
int gizmo_builtin_sound_rewind( lua_State *L ){

    lua_Integer lnVIndexID = lua_tointeger( L, -1 );
	gizmo_OpenAL->Rewind( (int)lnVIndexID );

    //XPLMDebugString("Gizmo: sound.play()\n");

    return 0;
}



/**
 sound.stop( sound_id )
 
 Stops playback for a given sound, moves playhead back to start of sample.
 */
int gizmo_builtin_sound_stop( lua_State *L ){

    lua_Integer lnVIndexID = lua_tointeger( L, -1 );
	gizmo_OpenAL->Stop( (int)lnVIndexID );

    //XPLMDebugString("Gizmo: sound.play()\n");

    return 0;
}



/**
 sound.setDirection( sound_id, nx,ny,nz )
 
 Controls the direction vector that the sound is travelling in.
 */
int gizmo_builtin_sound_setDirection( lua_State *L ){

	gizmo_OpenAL->SetDirection( lua_tointeger( L, -4 ), 
								lua_tonumber( L, -3 ),
								lua_tonumber( L, -2 ),
								lua_tonumber( L, -1 )
								);

	return 0;
}



/**
 sound.setGain( sound_id, gain_ratio )
 
 Controls the gain, or volume, of a sound sample, argument is a ratio.
 */
int gizmo_builtin_sound_setGain( lua_State *L ){

	lua_Integer lnVIndexID = lua_tointeger( L, -2 );
	lua_Number lnVGainVal = lua_tonumber( L, -1 );
	gizmo_OpenAL->SetGain( lnVIndexID, lnVGainVal );

	return 0;
}



/**
 sound.setLoop( sound_id, loop_flag )
 
 Set loop_flag to 1 and the sound will play forever until stopped or paused.
 Set loop_flag to 0 and the sound will stop playing when the end of the sample is reached.
 */
int gizmo_builtin_sound_setLoop( lua_State *L ){

	lua_Integer lnVIndexID = lua_tointeger( L, -2 );
	lua_Integer lnVLoopFlag = lua_tointeger( L, -1 );
	gizmo_OpenAL->SetLooping( lnVIndexID, lnVLoopFlag );

	return 0;
}



/**
 sound.setPitch( sound_id, pitch_ratio )
 
 Alter the pitch ratio of a given sound.
 0.5 will play at half pitch.
 2.0 will play at twice-normal pitch.
 1.0 will play normally.
 */
int gizmo_builtin_sound_setPitch( lua_State *L ){

	lua_Integer lnVIndexID = lua_tointeger( L, -2 );
	lua_Number lnVPitchVal = lua_tonumber( L, -1 );
	gizmo_OpenAL->SetPitch( lnVIndexID, lnVPitchVal );

	return 0;
}




/**
 sound.setPosition( sound_id, x,y,z )
 
 Set the sounds position in 3D space.
 
 Mono files only for 3D sound-stage use.
 */
int gizmo_builtin_sound_setPosition( lua_State *L ){

	gizmo_OpenAL->SetPosition( lua_tointeger( L, -4 ), 
								lua_tonumber( L, -3 ),
								lua_tonumber( L, -2 ),
								lua_tonumber( L, -1 )
								);

	return 0;
}



/**
 sound.setVelocity( sound_id, vx,vy,vz )
 
 Controls the velocity of the sound in 3D space.
 Combine this with sound.setDirection(...)
 
 */
int gizmo_builtin_sound_setVelocity( lua_State *L ){

	gizmo_OpenAL->SetVelocity( lua_tointeger( L, -4 ), 
								lua_tonumber( L, -3 ),
								lua_tonumber( L, -2 ),
								lua_tonumber( L, -1 )
								);

	return 0;
}





