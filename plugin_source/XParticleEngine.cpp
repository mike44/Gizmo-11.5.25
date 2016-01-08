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



#include "XParticleEngine.h"




double XParticleEngine::uniform()
{
	return static_cast<double>(std::rand())/static_cast<double>(RAND_MAX);
}



void XParticleEngine::update( float fFrp, float elapsed_sim_time ){
	
	this->last_elapsed_sim_time = this->elapsed_sim_time;
	this->elapsed_sim_time = elapsed_sim_time;

	
	std::vector<int> svCleanups;
	
	//perform physics updates
	for( unsigned int ipx=0; ipx < this->particles.size(); ipx++ ){
		this->particles[ipx]->update( fFrp ); //run particle physics update.
		
		if( this->particles[ipx]->Core.life_remaining <= 0.0f ){						
			svCleanups.push_back(ipx);
			//this->particles.erase( this->particles.begin() + ipx );
		}//check for dead particles that need restarting
	
	}//loop all particles and update them
	
	
	
	
	
	//DONT TOUCH THIS CODE, it crashes the sim if its wrong. KTHNX.
	if( svCleanups.size() > 0 ){

		//pool size doesnt seem to grow much above 5, calling it done.
	
		//char caDbg[1024];
		//sprintf(caDbg, "Gizmo: Particles to cleanup: %i\n", svCleanups.size());
		//XPLMDebugString( caDbg );
	
		for(unsigned int x=svCleanups.size(); x > 0; x--){
			if( svCleanups[x] < this->particles.size() ){
				//sprintf(caDbg, "Gizmo: Would like to erase particle: %i\n", svCleanups[x-1]);
				//XPLMDebugString( caDbg );
				this->particles.erase( this->particles.begin() + svCleanups[x] );
			}
		}
		svCleanups.clear();
	}
	
	
	
	
	

	if( this->enabled ){
		//if we have removed dead particles above and fallen below the max particles count we will attempt to create more!
		if( this->particles.size() < (unsigned int)this->count ){
			this->initParticles( fFrp ); //particle reset! endless growing. Yay.
		}
	}
		


}



void XParticleEngine::initParticles( float fFrp ){
	
	
	float time_delta = this->elapsed_sim_time - this->last_elapsed_sim_time;	
	float births = 0;
	
	
	//3D Interpolator.
	int x=1;
	while( births / time_delta < this->birth_rate ){
		
		births += 1.f;
		
		
		float d = XTrig::distanceBetweenPoints3D( this->last_zpX, this->last_zpY, this->last_zpZ,    this->zpX, this->zpY, this->zpZ ); 
		//float d = XTrig::distanceBetweenPoints3D( this->zpX, this->zpY, this->zpZ,   this->last_zpX, this->last_zpY, this->last_zpZ ); 
		
		float interp_x = x * (d/(float)x); //how far along the interp path are we?
		
		float dx = this->last_zpX - this->zpX;
		float dy = this->last_zpY - this->zpY;
		float dz = this->last_zpZ - this->zpZ;
		
		float np,nh;
		XTrig::computeCourseToLocation3D( dx, dy, dz, &np, &nh );
		
		float nx,ny,nz;
		XTrig::computeNewPositionOffset3D( nh, np, interp_x, &nx, &nz, &ny );
		
		float tx, ty, tz;
		tx = this->last_zpX + nx;
		ty = this->last_zpY + ny;
		tz = this->last_zpZ + nz;
		
		this->initParticle_Core(fFrp, tx, ty, tz, x ); 
		
		x++;
	
	}//end particle spawn inerpolator loop
	

	
	
	
}

void XParticleEngine::initParticle_Core( float fFrp, float x, float y, float z, int iInterStep ){

		
	XParticle *p = new XParticle();
	p->Core.life = this->life;
	p->Core.life_remaining = this->life;
	p->Core.sticky = this->sticky;
	this->particles.push_back( p );

	

	
	if( this->iType_Cubic ){
		this->initParticle_Cubic( p, fFrp );
	}

	//we are currently using this particle mode by default.
	if( this->iType_Spherical ){
		this->initParticle_Spherical( p, fFrp );
	}

	if( this->iType_Stream ){
		this->initParticle_Stream( p, fFrp );
	}

	
	
	p->Core.texture_id = this->iTextureId;

	p->Core.bounce = this->bounce;
	p->Core.gravity = this->gravity;
	p->Core.growth_rate = this->growth_rate;
	p->Core.drag = this->drag;
	p->Core.size = this->size;
	
	p->Core.sticky = this->sticky;

	p->Core.life_remaining = this->life;
	
	
	p->Core.rgba[0] = this->rgba[0];
	p->Core.rgba[1] = this->rgba[1];
	p->Core.rgba[2] = this->rgba[2];
	p->Core.rgba[3] = this->rgba[3];
	

	p->Core.x = x;
	p->Core.y = y;
	p->Core.z = z;
	
	
	p->Core.hard_deck			= this->hard_deck;
	p->Core.hard_deck_aware		= this->hard_deck_aware;
	
	
	
	//accelerte death effects.
	for( int x=0; x<iInterStep; x++ ){
		p->update( fFrp );
	}
	

}





void XParticleEngine::initParticle_Cubic( XParticle *p, float fFrp ){
	
	float rVx, rVy, rVz;
	float rAx, rAy, rAz;
	
	//cubic init
	rVy = this->uniform() * this->fEnergyMax;
	rVy = rVy * 0.5f;
	rAy = (rVy * 0.5f) * -1.0f;
	
	rVx = this->uniform() * this->fEnergyMax;
	rVx = rVx * 0.5f;
	rAx = (rVx * 0.5f) * -1.0f;
	
	rVz = this->uniform() * this->fEnergyMax;
	rVz = rVz * 0.5f;
	rAz = (rVz * 0.5f) * -1.0f;
	
	
	
	
	
	
	//this code inverts energy on a random basis so that we cover all hemispheres.
	float sw=0.f; //random switch
	if( ! this->iType_Stream ){
		
		sw = this->uniform();
		//printf("sw: %f\n", sw);
		if( sw > 0.5 ){
			rAx = rAx * -1.0f;
			rVx = rVx * -1.0f;
		}
		
		sw = this->uniform();
		//printf("sw: %f\n", sw);
		if( sw > 0.5 ){
			rAy = rAy * -1.0f;
			rVy = rVy * -1.0f;
		}
		
		sw = this->uniform();
		//printf("sw: %f\n", sw);
		if( sw > 0.5 ){
			rAz = rAz * -1.0f;
			rVz = rVz * -1.0f;
		}
	}
	
	
	p->Core.vy = (float)rVy;
	p->Core.ay = (float)rAy;
	
	p->Core.vx = (float)rVx;
	p->Core.ax = (float)rAx;
	
	p->Core.vz = (float)rVz;
	p->Core.az = (float)rAz;
	
		
	
}







void XParticleEngine::initParticle_Spherical( XParticle *p, float fFrp ){
	
	//random velocity and accel values to be generated.
	float rVx, rVy, rVz;
	float rAx, rAy, rAz;
	
	
	float fEnergySpread = this->fEnergyMax - this->fEnergyMin;
	float fEnergy = this->fEnergyMin + (fEnergySpread * this->uniform());
	
	
	XTrig::computeNewPositionOffset3D(
									  360.f * this->uniform(),	//heading
									  90.f * this->uniform(),	//pitch
									  fEnergy,	//speed
									  &rVx,
									  &rVy,
									  &rVz
									  );
	
	
	rAy = (rVy * 0.5f) * -1.0f;
	rAx = (rVx * 0.5f) * -1.0f;
	rAz = (rVz * 0.5f) * -1.0f;
	
	
	
	
	
	//this code inverts energy on a random basis so that we cover all hemispheres.
	float sw=0.f; //random switch
	if( ! this->iType_Stream ){
		
		sw = this->uniform();
		//printf("sw: %f\n", sw);
		if( sw > 0.5 ){
			rAx = rAx * -1.0f;
			rVx = rVx * -1.0f;
		}
		
		sw = this->uniform();
		//printf("sw: %f\n", sw);
		if( sw > 0.5 ){
			rAy = rAy * -1.0f;
			rVy = rVy * -1.0f;
		}
		
		sw = this->uniform();
		//printf("sw: %f\n", sw);
		if( sw > 0.5 ){
			rAz = rAz * -1.0f;
			rVz = rVz * -1.0f;
		}
	}
	
	
	
	//move computed values into particle instance.
	p->Core.vy = (float)rVy;
	p->Core.ay = (float)rAy;
	
	p->Core.vx = (float)rVx;
	p->Core.ax = (float)rAx;
	
	p->Core.vz = (float)rVz;
	p->Core.az = (float)rAz;
		
	
}








void XParticleEngine::initParticle_Stream( XParticle *p, float fFrp ){
	
	return;
	
	
	#warning stream particles generator is broken.
	
	
	float rVx, rVy, rVz;
	//float rAx, rAy, rAz;
	
	float fEnergySpread = this->fEnergyMax - this->fEnergyMin;
	float fEnergy = this->fEnergyMin + (fEnergySpread * this->uniform());
	
	//XTrig xtrig;
	XTrig::computeNewPositionOffset3D(
									  90.f, // this->fStreamHeading,
									  this->fStreamPitch+90.f,	
									  fEnergy,			
									  &rVx,
									  &rVy,
									  &rVz
									  );
	
	
	//wtf?
	//rAx = (rVx * 0.5f) * -1.0f;
	//rAy = (rVy * 0.5f) * -1.0f;
	//rAz = (rVz * 0.5f) * -1.0f;
	
	
	
	
	
	p->Core.vx = (float)rVx;
	//p->Core.ax = (float)rAx;
	
	p->Core.vy = (float)rVy;
	//p->Core.ay = (float)rAy;
	
	p->Core.vz = (float)rVz;
	//p->Core.az = (float)rAz;
	
	
	
}




