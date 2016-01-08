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


#ifndef __X_PARTICLE_ENGINE__
#define __X_PARTICLE_ENGINE__




//usefull STL classes
#include "X_STL.h"

#include "XTrig.h"


//#include "XDataRef.h"

#include "XPLMGraphics.h"

//OpenGL
#if APL
	//framework path differs
	#include <OpenGL/glu.h>
#else
	#include <GL/glu.h>
#endif


#include "XParticle.h"






class XParticleEngine{
	
public:
	
	XParticleEngine(){
		this->enabled = 0;
		//this->rgba = {1.f, 1.f, 1.f, 1.f};
		this->ready_for_drawing = 0;
		this->count = 0;
		
		this->zpX = 0.f;
		this->zpY = 0.f;
		this->zpZ = 0.f;
		
		this->last_zpX = 0.f;
		this->last_zpY = 0.f;
		this->last_zpZ = 0.f;
		
		this->zpX_Offset = 0.f;
		this->zpY_Offset = 0.f;
		this->zpZ_Offset = 0.f;
					
		this->sticky = 0;
		
		
		this->birth_rate = 0.f;
		this->elapsed_sim_time = 0.f;
		this->last_elapsed_sim_time = 0.f;
		this->births = 0.f;
		
		this->bounce = 0.f;
		this->drag = 0.f;
		this->gravity = 0.f;
		
		this->growth_rate = 0.f;
		
		this->life = 0.f;
		
		this->size = 0.f;
		
			this->iType_Cubic = 0;
			this->iType_Spherical = 0;
			this->iType_Stream = 0;
		
		this->iTextureId = 0;
		
		this->fEnergyMax = 0.f;
		this->fEnergyMin = 0.f;
		
		this->fStreamPitch = 0.f;
		this->fStreamHeading = 0.f;			
		this->fStreamNoise = 0.f;
		
		
		
		this->hard_deck_aware = 0;
		this->hard_deck = 0.f;
		
		
	};

	virtual ~XParticleEngine(){};

	
	virtual double uniform();


	virtual void initParticles( float fFrp );							//used in constructor
	virtual void initParticle_Core( float fFrp, float x, float y, float z, int iInterStep );
	
	virtual void initParticle_Cubic( XParticle *p, float fFrp );							//used in constructor
	virtual void initParticle_Spherical( XParticle *p, float fFrp );							//used in constructor
	virtual void initParticle_Stream( XParticle *p, float fFrp );							//used in constructor


	virtual void setCount( int c ){
		this->ready_for_drawing = 0; //particles need to be rebooted.
		this->count = c;
		for( unsigned int x=0; x<this->particles.size(); x++ ){
			delete( this->particles[x] );
		}
		this->particles.resize(0);
	};



	virtual void setBirthRate( float f ){ this->birth_rate = f; };
	virtual void setBounce( float f ){ this->bounce = f; };
	virtual void setDrag( float f ){ this->drag = f; };
	virtual void setEnergyMax( float f ){ this->fEnergyMax = f; };
	virtual void setEnergyMin( float f ){ this->fEnergyMin = f; };
	virtual void setGravity( float f ){ this->gravity = f; };
	virtual void setHardDeck( float f ){ this->hard_deck = f; this->hard_deck_aware = 1; };
	virtual void setGrowthRate( float f ){ this->growth_rate = f; };
	
	virtual void setLife( float f ){ this->life = f; };
	virtual void setSize( float f ){ this->size = f; };

	virtual void setStreamPitch( float f ){ this->fStreamPitch = f; };
	virtual void setStreamHeading( float f ){ this->fStreamHeading = f; };
	virtual void setStreamNoise( float f ){ this->fStreamNoise = f; };

	virtual void setTexture( int i ){ this->iTextureId = i; };


	virtual void setType__Reset(){ this->iType_Cubic = 0; this->iType_Spherical = 0; this->iType_Stream = 0; };
	virtual void setType_Cubic(){ this->setType__Reset(); this->iType_Cubic = 1; };
	virtual void setType_Spherical(){ this->setType__Reset(); this->iType_Spherical = 1; };
	virtual void setType_Stream(){ this->setType__Reset(); this->iType_Stream = 1; };



	//this is used to attach the particle generator to a dynamic location.
	//particles, once generated, retain a world position of their own.
	virtual void setOrigin( float x, float y, float z ){
		this->last_zpX = this->zpX;
		this->last_zpY = this->zpY;
		this->last_zpZ = this->zpZ;
	
		this->zpX = x;
		this->zpY = y;
		this->zpZ = z;
	};

	virtual void setOrigin_Offset( float x, float y, float z ){
		this->zpX_Offset = x;
		this->zpY_Offset = y;
		this->zpZ_Offset = z;
	};


	virtual void start( float fFrp ){
		
		//Create 1 particle to kick off the engine.
		XParticle *p;
			p = new XParticle();
				p->Core.life = this->life;
				p->Core.life_remaining = 0.f;
				p->Core.sticky = this->sticky;
			this->particles.push_back( p );
		
		this->ready_for_drawing = 1; //toggle sub-sys ready state flag
		this->enabled = 1; //toggle emission
	};


	virtual void setEnabled( int flag ){
		this->enabled = flag;
	};


	virtual void setStuckToAcf( int b ){
		this->sticky = b;
	};


	virtual void update( float fFrp, float elapsed_sim_time );



	virtual void setColor( float r, float g, float b, float a ){
		this->rgba[0] = r;
		this->rgba[1] = g;
		this->rgba[2] = b;
		this->rgba[3] = a;			
	};





	int enabled;
	float rgba[4];
	int ready_for_drawing;

	std::vector<XParticle*> particles;
	int count;
	
	
	//zero point - auto updated from host plugin(or should be)
	float zpX, zpY, zpZ;
	float last_zpX, last_zpY, last_zpZ;

	//zero point offset - set once(usually) to provide an engine location offset etc.
	float zpX_Offset, zpY_Offset, zpZ_Offset;

	
	
	
	int sticky; //stuck to acf
	float birth_rate;
	float elapsed_sim_time;
	float last_elapsed_sim_time;
	float births;
	
	float hard_deck; //ground!
	int hard_deck_aware;
	
	
	float bounce; //how much energy to retain when we hit something
	float drag;
	float gravity; //how much to slow down in 1 due to gravity.
	float growth_rate;
	float life;
	float size;
	
	int iType_Cubic;
	int iType_Spherical;
	int iType_Stream;

	int iTextureId;

	float fEnergyMax;
	float fEnergyMin;
	
	float fStreamPitch;
	float fStreamHeading;
	float fStreamNoise;
	
};



#endif
