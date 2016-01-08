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


#ifndef __GIZMO_OPENAL__
#define __GIZMO_OPENAL__



#include "GlobalDefs.h"

#include <cstdlib>
#include <stdio.h>

#include "XPLMUtilities.h"

#include "X_STL.h"

extern "C" {
	void gizmo_ThrowScriptWarning(std::string);
};



#if APL
    #include <OpenAL/al.h>
    #include <OpenAL/alc.h>
    //#include <OpenAL/alext.h>
    //#include <OpenAL/alut.h>

#elif IBM
    #include <al.h>
    #include <alc.h>
    //#include <alext.h>
    //#include <AL/alut.h> // - we dont use this, Supnik gave us sound loader code.

#elif LIN
    #include <AL/al.h>
    #include <AL/alc.h>
    //#include <AL/alext.h>
    //#include <AL/alut.h>

#endif




#ifdef DMALLOC
#include "dmalloc.h"
#endif










/**************************************************************************************************************
 * WAVE FILE LOADING
 **************************************************************************************************************/

// You can just use alutCreateBufferFromFile to load a wave file, but there seems to be a lot of problems with
// alut not beign available, being deprecated, etc.  So...here's a stupid routine to load a wave file.  I have
// tested this only on x86 machines, so if you find a bug on PPC please let me know.

// Macros to swap endian-values.

#define SWAP_32(value)                 \
        (((((unsigned short)value)<<8) & 0xFF00)   | \
         ((((unsigned short)value)>>8) & 0x00FF))

#define SWAP_16(value)                     \
        (((((unsigned int)value)<<24) & 0xFF000000)  | \
         ((((unsigned int)value)<< 8) & 0x00FF0000)  | \
         ((((unsigned int)value)>> 8) & 0x0000FF00)  | \
         ((((unsigned int)value)>>24) & 0x000000FF))

// Wave files are RIFF files, which are "chunky" - each section has an ID and a length.  This lets us skip
// things we can't understand to find the parts we want.  This header is common to all RIFF chunks.
struct chunk_header {
	int			id;
	int			size;
};

// WAVE file format info.  We pass this through to OpenAL so we can support mono/stereo, 8/16/bit, etc.
struct format_info {
	short		format;				// PCM = 1, not sure what other values are legal.
	short		num_channels;
	int			sample_rate;
	int			byte_rate;
	short		block_align;
	short		bits_per_sample;
};

// This utility returns the start of data for a chunk given a range of bytes it might be within.  Pass 1 for
// swapped if the machine is not the same endian as the file.
static char *	find_chunk(char * file_begin, char * file_end, int desired_id, int swapped)
{
	while(file_begin < file_end)
	{
		chunk_header * h = (chunk_header *) file_begin;
		if(h->id == desired_id && !swapped)
			return file_begin+sizeof(chunk_header);
		if(h->id == SWAP_32(desired_id) && swapped)
			return file_begin+sizeof(chunk_header);
		int chunk_size = swapped ? SWAP_32(h->size) : h->size;
		char * next = file_begin + chunk_size + sizeof(chunk_header);
		if(next > file_end || next <= file_begin)
			return NULL;
		file_begin = next;
	}
	return NULL;
}

// Given a chunk, find its end by going back to the header.
static char * chunk_end(char * chunk_start, int swapped)
{
	chunk_header * h = (chunk_header *) (chunk_start - sizeof(chunk_header));
	return chunk_start + (swapped ? SWAP_32(h->size) : h->size);
}

#define FAIL(X) { XPLMDebugString(X); free(mem); return 0; }

#define RIFF_ID 0x46464952			// 'RIFF'
#define FMT_ID  0x20746D66			// 'FMT '
#define DATA_ID 0x61746164			// 'DATA'

ALuint load_wave(const char * file_name, ALuint buf_id)
{
	// First: we open the file and copy it into a single large memory buffer for processing.

	FILE * fi = fopen(file_name,"rb");
	if(fi == NULL)
	{
		XPLMDebugString("Gizmo: WAVE file load failed - could not open.\n");
		return 0;
	}
	fseek(fi,0,SEEK_END);
	unsigned int file_size = ftell(fi);
	fseek(fi,0,SEEK_SET);
	char * mem = (char*) malloc(file_size);
	if(mem == NULL)
	{
		XPLMDebugString("Gizmo: WAVE file load failed - could not allocate memory.\n");
		fclose(fi);
		return 0;
	}
	if (fread(mem, 1, file_size, fi) != file_size)
	{
		XPLMDebugString("Gizmo: WAVE file load failed - could not read file.\n");
		free(mem);
		fclose(fi);
		return 0;
	}
	fclose(fi);
	char * mem_end = mem + file_size;

	// Second: find the RIFF chunk.  Note that by searching for RIFF both normal
	// and reversed, we can automatically determine the endian swap situation for
	// this file regardless of what machine we are on.

	int swapped = 0;
	char * riff = find_chunk(mem, mem_end, RIFF_ID, 0);
	if(riff == NULL)
	{
		riff = find_chunk(mem, mem_end, RIFF_ID, 1);
		if(riff)
			swapped = 1;
		else
			FAIL("Gizmo: Could not find RIFF chunk in wave file.\n")
	}

	// The wave chunk isn't really a chunk at all. :-(  It's just a "WAVE" tag
	// followed by more chunks.  This strikes me as totally inconsistent, but
	// anyway, confirm the WAVE ID and move on.

	if (riff[0] != 'W' ||
		riff[1] != 'A' ||
		riff[2] != 'V' ||
		riff[3] != 'E')
		FAIL("Gizmo: Could not find WAVE signature in wave file.\n")

	char * format = find_chunk(riff+4, chunk_end(riff,swapped), FMT_ID, swapped);
	if(format == NULL)
		FAIL("Gizmo: Could not find FMT  chunk in wave file.\n")

	// Find the format chunk, and swap the values if needed.  This gives us our real format.

	format_info * fmt = (format_info *) format;
	if(swapped)
	{
		fmt->format = SWAP_16(fmt->format);
		fmt->num_channels = SWAP_16(fmt->num_channels);
		fmt->sample_rate = SWAP_32(fmt->sample_rate);
		fmt->byte_rate = SWAP_32(fmt->byte_rate);
		fmt->block_align = SWAP_16(fmt->block_align);
		fmt->bits_per_sample = SWAP_16(fmt->bits_per_sample);
	}

	// Reject things we don't understand...expand this code to support weirder audio formats.

	if(fmt->format != 1) FAIL("Gizmo: Wave file is not PCM format data.\n")
	if(fmt->num_channels != 1 && fmt->num_channels != 2) FAIL("Gizmo: Must have mono or stereo sound.\n")
	if(fmt->bits_per_sample != 8 && fmt->bits_per_sample != 16) FAIL("Gizmo: Must have 8 or 16 bit sounds.\n")
	char * data = find_chunk(riff+4, chunk_end(riff,swapped), DATA_ID, swapped) ;
	if(data == NULL)
		FAIL("Gizmo: I could not find the DATA chunk.\n")

	int sample_size = fmt->num_channels * fmt->bits_per_sample / 8;
	int data_bytes = chunk_end(data,swapped) - data;
	int data_samples = data_bytes / sample_size;

	// If the file is swapped and we have 16-bit audio, we need to endian-swap the audio too or we'll
	// get something that sounds just astoundingly bad!

	if(fmt->bits_per_sample == 16 && swapped)
	{
		short * ptr = (short *) data;
		int words = data_samples * fmt->num_channels;
		while(words--)
		{
			*ptr = SWAP_16(*ptr);
			++ptr;
		}
	}

	// Finally, the OpenAL crud.  Build a new OpenAL buffer and send the data to OpenAL, passing in
	// OpenAL format enums based on the format chunk.

	if(buf_id == 0) FAIL("Could not generate buffer id.\n");
	char caDbg[2048];
	sprintf(caDbg, "Gizmo: OpenAL buffer target for wav data: %i\n", buf_id);
	XPLMDebugString(caDbg);



	if( fmt->num_channels == 2 ){
		gizmo_ThrowScriptWarning("OpenAL: Stereo sound sample loaded: Cannot use this on a 3D sound stage."); //throw a warning into the script engine
				 XPLMDebugString("Gizmo: OpenAL WARNING: STEREO DATA: Will not render on 3D sound stage.\n"); //write info to the X-Plane Log.txt
	}



	alBufferData(buf_id, fmt->bits_per_sample == 16 ?
								(fmt->num_channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16) :
								(fmt->num_channels == 2 ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8),
					data, data_bytes, fmt->sample_rate);

	free(mem);
	return buf_id;
};







class GizmoWrappedSoundSource{
private:
	//foo
	
	float x,y,z;		//position
	float vx,vy,vz;		//velocity
	float nx,ny,nz;		//direction
	
	float audio_pitch;
	float audio_gain;
	
	int audio_loop_flag;
	int audio_play_state; //can be paused, stopped or playing.
	
	
	int audio_glued_to_acf;
	
	
	
public:
	
	const static int play_state_stopped = 0;
	const static int play_state_playing = 1;
	const static int play_state_paused = 2;
	
	
	
	GizmoWrappedSoundSource(){ 
		this->x = 0;
		this->y = 0;
		this->z = 0;
		
		this->vx = 0;
		this->vy = 0;
		this->vz = 0;

		this->nx = 0;
		this->ny = 0;
		this->nz = 0;
		
		this->audio_pitch = 0;
		this->audio_gain = 0;
		
		this->audio_loop_flag = 0;
		this->audio_play_state = this->play_state_stopped;
		
		
		this->audio_glued_to_acf = 1;
		
	}
	
	~GizmoWrappedSoundSource(){ 
		
	}
	
	

	// -- Instance control functions
	
	void Pause( int slotIndex ){
		//this->takeContext();
		//alSourcePause( this->uiSources[slotIndex] );
		//this->restoreContext();
	};
	
	void Play( int slotIndex ){
		//this->takeContext();
		//alSourcePlay( this->uiSources[slotIndex] );
		//this->restoreContext();
	};
	
	void Rewind( int slotIndex ){
		//this->takeContext();
		//alSourceRewind( this->uiSources[slotIndex] );
		//this->restoreContext();
	};
	
	void Stop( int slotIndex ){
		//this->takeContext();
		//alSourceStop( this->uiSources[slotIndex] );
		//this->restoreContext();
	};
	
	
	
	
	
	
	
	void SetDirection( int slotIndex, float vx, float vy, float vz ){
		//this->takeContext();
		//alSource3f(this->uiSources[slotIndex],AL_DIRECTION, vx, vy, vz);
		//this->restoreContext();
	};
	
	void SetGain( int slotIndex, float gainVal ){
		//this->takeContext();
		//alSourcef( this->uiSources[slotIndex], AL_GAIN, gainVal );
		//this->restoreContext();
	};
	
	void SetLooping( int slotIndex, int flag ){
		//this->takeContext();
		//alSourcei( this->uiSources[slotIndex], AL_LOOPING, flag );
		//this->restoreContext();
	};
	
	void SetPitch( float pitchVal ){
		//this->takeContext();
		//alSourcef( this->uiSources[slotIndex], AL_PITCH, pitchVal );
		//this->restoreContext();
	};
	
	void SetPosition( float x, float y, float z ){
		//this->takeContext();
		//alSource3f(this->uiSources[slotIndex],AL_POSITION, x, y, z);
		//this->restoreContext();
	};
	
	void SetVelocity( float vx, float vy, float vz ){
		//this->takeContext();
		//alSource3f(this->uiSources[slotIndex],AL_VELOCITY, vx, vy, vz);
		//this->restoreContext();
	};
	
	
	
	
};







class GizmOpenALHooks{
	private:
		ALCcontext		*lastContext;

		ALCcontext      *Context;
		ALCdevice       *Device;

        int iSoundSlotCursor;
		int iMaxSoundSlots;

		ALuint          uiBuffer[256];
		ALuint          uiSources[256];
	
	
		float last_cam_x, last_cam_y, last_cam_z,	last_cam_p, last_cam_h, last_cam_r;
	


	public:
		GizmOpenALHooks(){

			this->lastContext = alcGetCurrentContext();
			
			this->Context = NULL;
			this->Device = NULL;

            this->iSoundSlotCursor = 0;
			this->iMaxSoundSlots = 256;

			
			if( this->lastContext == NULL ){
			
				#if IBM
					XPLMDebugString("\t OpenAL Win: Init..\n");
					//Device = alcOpenDevice((ALchar*)"DirectSound3D");
					this->Device = alcOpenDevice((ALchar*)"");
				#endif

				#if APL
					XPLMDebugString("\t OpenAL Mac: Init..\n");
					this->Device = alcOpenDevice((ALchar*)"");
				#endif

				#if LIN
					XPLMDebugString("\t OpenAL Lin: Init..\n");
					this->Device = alcOpenDevice((ALchar*)"");
				#endif


                if (this->Device == NULL ){
                    //printf("XLua: OpenAL Device == NULL.. critical error.\n");
                    XPLMDebugString("Gizmo: OpenAL Device == NULL: Critical sound error.\n");
                    //exit(-1);

                }else{

					//Create context(s)
					XPLMDebugString("Gizmo: OpenAL: Creating new context..\n");
					this->Context=alcCreateContext(this->Device, NULL);
				}
				
			}else{
				//we're using one big ass shared context, OpenAL already inited, etc.
				XPLMDebugString("Gizmo: OpenAL: Using shared OpenAL context..\n");
				
				this->Context = this->lastContext;
			}

			
			
			
						this->takeContext();
			
							//XPLMDebugString("Gizmo: OpenAL: Generating buffers and sources..\n");
							alGenBuffers( 256, this->uiBuffer );
							alGenSources( 256, this->uiSources );

							//TODO: Error checking to ensure buffers and sources were created?


							// Clear Error Code
							//XPLMDebugString("Gizmo: OpenAL: Clearing error code..\n");
							alGetError();

						//XPLMDebugString("Gizmo: OpenAL: Restoring original context.\n");
						this->restoreContext();


		};

		~GizmOpenALHooks(){
			this->takeContext();

				//free buffers and sources
					//delete our sources
						alDeleteSources(256,this->uiSources);
					//delete our buffers
						alDeleteBuffers(256,this->uiBuffer);

				if( this->lastContext != this->Context ){
					
					XPLMDebugString("Gizmo: Cleaning up OpenAL: Closing context and device.\n");
					
					//Get device for active context
					this->Device=alcGetContextsDevice(this->Context);
					//Disable context
					//alcMakeContextCurrent(NULL);
					//Release context(s)
					alcDestroyContext(this->Context);
					//Close device
					alcCloseDevice(this->Device);
				}

			this->restoreContext();

		};







		void LoadSound( int slotIndex, const char* filename ){
			this->takeContext();
				#if __POWERPC__
					XPLMSpeakString("Sound loading is disabled on PowerPC. Your hardware is too old to support. Sorry. -Gizmo");
					XPLMDebugString("Gizmo: Sound loading is disabled on PowerPC. Your hardware is too old to support. Sorry.\n");
				#else
					load_wave(filename, this->uiBuffer[slotIndex]);
				#endif

				alSourcei(this->uiSources[slotIndex], AL_BUFFER, 0); //erase previous contents
				alSourcei(this->uiSources[slotIndex], AL_BUFFER, this->uiBuffer[slotIndex]);

			this->restoreContext();
		};


        int GetSoundSlot(){ int r = this->iSoundSlotCursor; this->iSoundSlotCursor++; return r; };
		void ResetSoundSlotCursor(){ this->iSoundSlotCursor = 0; };





		// -- Sound Source functions --
		void Pause( int slotIndex ){
			this->takeContext();
				alSourcePause( this->uiSources[slotIndex] );
			this->restoreContext();
		};

		void Play( int slotIndex ){
			this->takeContext();
				alSourcePlay( this->uiSources[slotIndex] );
			this->restoreContext();
		};

		void Rewind( int slotIndex ){
			this->takeContext();
				alSourceRewind( this->uiSources[slotIndex] );
			this->restoreContext();
		};

		void Stop( int slotIndex ){
			this->takeContext();
				alSourceStop( this->uiSources[slotIndex] );
			this->restoreContext();
		};



		void SetDirection( int slotIndex, float vx, float vy, float vz ){
			this->takeContext();
				alSource3f(this->uiSources[slotIndex],AL_DIRECTION, vx, vy, vz);
			this->restoreContext();
		};

		void SetGain( int slotIndex, float gainVal ){
			this->takeContext();
				alSourcef( this->uiSources[slotIndex], AL_GAIN, gainVal );
			this->restoreContext();
		};

        void SetLooping( int slotIndex, int flag ){
			this->takeContext();
				alSourcei( this->uiSources[slotIndex], AL_LOOPING, flag );
			this->restoreContext();
        };

		void SetPitch( int slotIndex, float pitchVal ){
			this->takeContext();
				alSourcef( this->uiSources[slotIndex], AL_PITCH, pitchVal );
			this->restoreContext();
		};

		void SetPosition( int slotIndex, float x, float y, float z ){
			this->takeContext();
				alSource3f(this->uiSources[slotIndex],AL_POSITION, x, y, z);
			this->restoreContext();
		};

		void SetVelocity( int slotIndex, float vx, float vy, float vz ){
			this->takeContext();
				alSource3f(this->uiSources[slotIndex],AL_VELOCITY, vx, vy, vz);
			this->restoreContext();
		};






		// --- OpenAL helper functions --
	
		void takeContext(){
			this->saveContext();
			alcMakeContextCurrent(this->Context);
		};

		void saveContext(){
			this->lastContext = alcGetCurrentContext();
		};

		void restoreContext(){
			if( this->lastContext != NULL ){
				alcMakeContextCurrent(this->lastContext);
				this->lastContext = NULL;
			}
		};

	
	
	
	
		// -- Nasty ass world physics updating code.
	
		
		/**
		 This function is responsible for updating all the matrix math and updating OpenAL with sanitized position values etc.
		 OpenAL is "crap" in that the default internals are reversed for a useful audio engine.
		 - That is... OpenAL gives us tools to set the source positions and then move the listener.
		 - To do it correctly we have to redo the math, keeping the listener still and moving the sources 
		 to simulate how the listener would normally move.
		*/
		void calculateAudioStage( 
								 float cam_x, float cam_y, float cam_z, 
								 float cam_p, float cam_r, float cam_h 
								 ){
			//The listener is kept at X-Planes camera location.
			
			//We need to calculate the delta on x-planes last camera position?
			
			float delta_cam_x = cam_x - this->last_cam_x;
			float delta_cam_y = cam_y - this->last_cam_y;
			float delta_cam_z = cam_z - this->last_cam_z;
			
			float delta_cam_p = cam_p - this->last_cam_p;
			float delta_cam_r = cam_r - this->last_cam_r;
			float delta_cam_h = cam_h - this->last_cam_h;
			
			
			//we want to move _every_ sound source by the deltas provided above. AWESOME. -_-
			
		
		
		};
		

};



#endif
