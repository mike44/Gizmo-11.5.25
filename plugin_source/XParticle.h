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


#ifndef __PARTICLE__
#define __PARTICLE__

#include "X_STL.h"

//#include "XDataRef.h"

#include <stdio.h>
#include <string.h>


class XParticle{
	public:

		//these are wrapped in a struct so that we can memcpy them out for max speed.
		struct{
			float x, y, z;			//position
			float ax, ay, az;		//acceleration
			float vx, vy, vz;		//velocity
			float ovx, ovy, ovz;	//original velocity, used for recycling particles.
			
			float off_x, off_y, off_z; //offset position, passed in during collation.

			float camera_distance;
			float heading_to_camera;
			float pitch_to_camera;
			
			int sticky; //stuck to the users acf.

			int texture_id;

			float bounce; //how much energy to retain when we hit something
			
			
			float drag;
			float gravity; //how much to slow down in 1 due to gravity.
			
			float size;
			float growth_rate;
			
			//float mass; //gives us inertia.
			
			float life;
			float life_remaining;
			
			
			float hard_deck;
			int hard_deck_aware;
			
			
			
			float rgba[4];
			
		} Core;
		
	
	
	
	
		
		XParticle(){
			this->Core.x = 0.f;
			this->Core.y = 0.f;
			this->Core.z = 0.f;

			this->Core.ax = 0.f;
			this->Core.ay = 0.f;
			this->Core.az = 0.f;

			this->Core.vx = 0.f;
			this->Core.vy = 0.f;
			this->Core.vz = 0.f;

			this->Core.ovx = 0.f;
			this->Core.ovy = 0.f;
			this->Core.ovz = 0.f;

			this->Core.off_x = 0.f;
			this->Core.off_y = 0.f;
			this->Core.off_z = 0.f;
			
			
			this->Core.camera_distance = 0.f;
			this->Core.heading_to_camera = 0.f;
			this->Core.pitch_to_camera = 0.f;
			
			this->Core.sticky = 0;			
			this->Core.texture_id = 0;			
			this->Core.bounce = 0.f;
			
			this->Core.drag = 0.f;
			
			this->Core.gravity = 0.f;
			
			this->Core.size = 0.f;
			
			this->Core.growth_rate = 0.f;
			
			this->Core.life = 0.f;
			this->Core.life_remaining = this->Core.life;

		
			this->Core.rgba[0] = 1.f;
			this->Core.rgba[1] = 1.f;
			this->Core.rgba[2] = 1.f;
			this->Core.rgba[3] = 1.f;
		
		};
		
	
	virtual ~XParticle(){};
	
	
		
		virtual void update( float fFrp ){
			//dead particles dont run
				if( this->Core.life_remaining < 0.f ){ return; }

			//drag
				float d = this->Core.drag * fFrp;

				if(this->Core.vy < 0.0f ){
					this->Core.vy += d;
				}else{
					this->Core.vy -= d;
				}
				if(this->Core.vx < 0.0f ){
					this->Core.vx += d;
				}else{
					this->Core.vx -= d;
				}
				if(this->Core.vz < 0.0f ){
					this->Core.vz += d;
				}else{
					this->Core.vz -= d;
				}


			//position
				this->Core.x += (this->Core.vx * fFrp);
				this->Core.y += (this->Core.vy * fFrp);
				if( this->Core.hard_deck_aware ){
					if(this->Core.y < this->Core.hard_deck){						
						this->Core.y = this->Core.hard_deck; //clamp to ground.
						this->Core.gravity = 0.f; //kill gravity
						this->Core.vy = 0.f; //kill vertical velocity.						
					}					
				}
				this->Core.z += (this->Core.vz * fFrp);


			//gravity
				this->Core.vy += (this->Core.gravity * fFrp);


			//death
				this->Core.life_remaining -= (1.f * fFrp);
				//if( this->Core.life_remaining < 0.f ){ this->Core.life_remaining = 0.f; }
			


			//size
				this->Core.size += (this->Core.growth_rate * fFrp);
				if( this->Core.size < 0.f ){ 
					this->Core.size = 0.f; 
					this->Core.life_remaining = 0.f;
				}

		};


	
	
	
	


		virtual void Copy( XParticle* s ){			
			memcpy( &(this->Core), &(s->Core), sizeof(this->Core));
		};
		

};



#endif
