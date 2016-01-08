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


#ifndef __GIZMO_BULLET_HOOKS__
#define __GIZMO_BULLET_HOOKS__



#include "GlobalDefs.h"



//bullet physics lib
#include "../third_party_source/bullet-2.77/src/btBulletDynamicsCommon.h"
//#include "btParallelConstraintSolver.h"
#include "../third_party_source/bullet-2.77/Demos/OpenGL/GLDebugDrawer.h"
//#include "btSoftRigidDynamicsWorld.h"



#include "XPLMUtilities.h"




#ifdef DMALLOC
#include "dmalloc.h"
#endif



#include <stdio.h>


class GizmoBulletHooks{
	public:
		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;

		//the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		btSequentialImpulseConstraintSolver* solver;
		//btParallelConstraintSolver* solver;

		btDiscreteDynamicsWorld* dynamicsWorld;
		btCollisionObjectArray dynamicsWorld_ObjectArray;

		btAxisSweep3* overlappingPairCache;

		btAlignedObjectArray<btCollisionShape*> collisionShapes;


		GLDebugDrawer	gDebugDrawer;


		GizmoBulletHooks();
		virtual ~GizmoBulletHooks();
		virtual void CreateDynamicObject( btCollisionShape* colShape );






		#if GIZMO_BULLET_COLLADA_ENABLE
		//this function dumps a snapshot of the physics world into a collada file, handy!
		virtual void ColladaSave(){
			ColladaConverter converter( this->dynamicsWorld );
			converter.save("snapshot.dae"); //store it in the current working directory

		};


		virtual void ColladaLoad(){
			ColladaConverter converter( this->dynamicsWorld );
			converter.load("snapshot.dae"); //store it in the current working directory

		};
		#endif




		virtual void Update( float fTimeDelta ){

			dynamicsWorld->stepSimulation( fTimeDelta, this->iSimSteps);

		};






		// -- General World Functions --
			virtual void SetCursor( float x, float y, float z ){
				this->fCursorX = x;
				this->fCursorY = y;
				this->fCursorZ = z;
			};

			virtual void SetIntertia( float x, float y, float z ){
				this->fInertiaX = x;
				this->fInertiaY = y;
				this->fInertiaZ = z;
			};

			virtual void SetMass( float m ){ this->fMass = m; };

			virtual void SetGravityVector( float x, float y, float z ){
				dynamicsWorld->setGravity(btVector3(x,y,z));
			};




		// -- Primitives --

			virtual void CreateSphere( float r ){
				btCollisionShape* colShape = new btSphereShape(btScalar( r ));
				this->CreateDynamicObject( colShape );
			};

			virtual void CreateBox( float x, float y, float z ){
				btCollisionShape* colShape = new btBoxShape(btVector3(x,y,z));
				this->CreateDynamicObject( colShape );
			};

			//new below here - bind to lua.
			virtual void CreateCylinder( float x, float y, float z ){
				btCollisionShape* colShape = new btCylinderShape(btVector3(x,y,z));
				this->CreateDynamicObject( colShape );
			};

			virtual void CreateCapsule( float r, float h ){
				btCollisionShape* colShape = new btCapsuleShape(btScalar(r), btScalar(h));
				this->CreateDynamicObject( colShape );
			};

			virtual void CreateCone( float r, float h ){
				btCollisionShape* colShape = new btConeShape(btScalar(r), btScalar(h));
				this->CreateDynamicObject( colShape );
			};

			//btMultiSphereShape




		// -- Mesh Objects --

			//btConvexHull

			//btConvexTriangleMeshShape

			//btBvhTriangleMeshShape

			//btHeightfieldTerrainShape

			//btStaticPlaneShape






		// -- Object Control / Influence / Force Application and Reading

			//apply
				//CentralForce
				virtual void rb_applyCentralForce( int objectId,  float a, float b, float c ){

					btVector3 param = btVector3( a, b, c );

					this->dynamicsWorld_ObjectArray = this->dynamicsWorld->getCollisionObjectArray();

					btCollisionObject* obj = this->dynamicsWorld_ObjectArray[ objectId ];
					btRigidBody* body = btRigidBody::upcast(obj);

					body->applyCentralForce( param );

				};

				//CentralImpulse
				virtual void rb_applyCentralImpulse( int objectId,  float a, float b, float c ){

					btVector3 param = btVector3( a, b, c );

					this->dynamicsWorld_ObjectArray = this->dynamicsWorld->getCollisionObjectArray();

					btCollisionObject* obj = this->dynamicsWorld_ObjectArray[ objectId ];
					btRigidBody* body = btRigidBody::upcast(obj);

					body->applyCentralImpulse( param );

				};

				//TorqueImpulse
				virtual void rb_applyTorqueImpulse( int objectId,  float a, float b, float c ){

					btVector3 param = btVector3( a, b, c );

					this->dynamicsWorld_ObjectArray = this->dynamicsWorld->getCollisionObjectArray();

					btCollisionObject* obj = this->dynamicsWorld_ObjectArray[ objectId ];
					btRigidBody* body = btRigidBody::upcast(obj);

					body->applyTorqueImpulse( param );

				};


				//Impulse -- accepts a location input as well
				virtual void rb_applyImpulse( int objectId, float* user_param, float* user_param_b ){

					btVector3 param = btVector3( user_param[0], user_param[1], user_param[2] );
					btVector3 param_b = btVector3( user_param_b[0], user_param_b[1], user_param_b[2] );

					this->dynamicsWorld_ObjectArray = this->dynamicsWorld->getCollisionObjectArray();

					btCollisionObject* obj = this->dynamicsWorld_ObjectArray[ objectId ];
					btRigidBody* body = btRigidBody::upcast(obj);

					body->applyImpulse( param, param_b );

				};


				//Force -- accepts a location input as well
				virtual void rb_applyForce( int objectId, float* user_param, float* user_param_b ){

					btVector3 param = btVector3( user_param[0], user_param[1], user_param[2] );
					btVector3 param_b = btVector3( user_param_b[0], user_param_b[1], user_param_b[2] );

					this->dynamicsWorld_ObjectArray = this->dynamicsWorld->getCollisionObjectArray();

					btCollisionObject* obj = this->dynamicsWorld_ObjectArray[ objectId ];
					btRigidBody* body = btRigidBody::upcast(obj);

					body->applyForce( param, param_b );

				};




				//Torque
				virtual void rb_applyTorque( int objectId,  float a, float b, float c ){

					btVector3 param = btVector3( a, b, c );

					this->dynamicsWorld_ObjectArray = this->dynamicsWorld->getCollisionObjectArray();

					btCollisionObject* obj = this->dynamicsWorld_ObjectArray[ objectId ];
					btRigidBody* body = btRigidBody::upcast(obj);

					body->applyTorque( param );

				};




			//clearForces
				virtual void rb_clearForces( int objectId ){
					this->dynamicsWorld_ObjectArray = this->dynamicsWorld->getCollisionObjectArray();

					btCollisionObject* obj = this->dynamicsWorld_ObjectArray[ objectId ];
					btRigidBody* body = btRigidBody::upcast(obj);

					body->clearForces();

				};

			//getOrientationQ
				virtual void rb_getOrientationQ( int objectId, float* user_return_param ){
					this->dynamicsWorld_ObjectArray = this->dynamicsWorld->getCollisionObjectArray();

					btCollisionObject* obj = this->dynamicsWorld_ObjectArray[ objectId ];
					btRigidBody* body = btRigidBody::upcast(obj);

					btQuaternion tmp_ret = body->getOrientation();

					if( sizeof( user_return_param ) == sizeof(float)*4 ){
						btVector3 q_vect = tmp_ret.getAxis();
						btScalar q_angle = tmp_ret.getAngle();

						user_return_param[0] = q_vect[0];
						user_return_param[1] = q_vect[1];
						user_return_param[2] = q_vect[2];

						user_return_param[3] = q_angle;

					}

				};

			//getLinearVelocity
				virtual void rb_getLinearVelocity( int objectId, float* user_return_param ){
					this->dynamicsWorld_ObjectArray = this->dynamicsWorld->getCollisionObjectArray();

					btCollisionObject* obj = this->dynamicsWorld_ObjectArray[ objectId ];
					btRigidBody* body = btRigidBody::upcast(obj);

					btVector3 tmp_ret = body->getLinearVelocity();

					if( sizeof( user_return_param ) == sizeof(float)*3 ){
						user_return_param[0] = tmp_ret[0];
						user_return_param[1] = tmp_ret[1];
						user_return_param[2] = tmp_ret[2];
					}

				};

			//getAngularVelocity
				virtual void rb_getAngularVelocity( int objectId, float* user_return_param ){
					this->dynamicsWorld_ObjectArray = this->dynamicsWorld->getCollisionObjectArray();

					btCollisionObject* obj = this->dynamicsWorld_ObjectArray[ objectId ];
					btRigidBody* body = btRigidBody::upcast(obj);

					btVector3 tmp_ret = body->getAngularVelocity();

					if( sizeof( user_return_param ) == sizeof(float)*3 ){
						user_return_param[0] = tmp_ret[0];
						user_return_param[1] = tmp_ret[1];
						user_return_param[2] = tmp_ret[2];
					}

				};

			//setLinearVelocity
				virtual void rb_setLinearVelocity( int objectId,   float a, float b, float c ){

					btVector3 param = btVector3( a, b, c );

					this->dynamicsWorld_ObjectArray = this->dynamicsWorld->getCollisionObjectArray();

					btCollisionObject* obj = this->dynamicsWorld_ObjectArray[ objectId ];
					btRigidBody* body = btRigidBody::upcast(obj);

					body->setLinearVelocity( param );

				};

			//setAngularVelocity
				virtual void rb_setAngularVelocity( int objectId,  float a, float b, float c ){

					btVector3 param = btVector3( a, b, c );

					this->dynamicsWorld_ObjectArray = this->dynamicsWorld->getCollisionObjectArray();

					btCollisionObject* obj = this->dynamicsWorld_ObjectArray[ objectId ];
					btRigidBody* body = btRigidBody::upcast(obj);

					body->setAngularVelocity( param );

				};

			//translate
				virtual void rb_translate( int objectId,  float a, float b, float c ){

					btVector3 param = btVector3( a, b, c );

					this->dynamicsWorld_ObjectArray = this->dynamicsWorld->getCollisionObjectArray();

					btCollisionObject* obj = this->dynamicsWorld_ObjectArray[ objectId ];
					btRigidBody* body = btRigidBody::upcast(obj);

					body->translate( param );

				};





	private:
		int iSimSteps; //bullet rec' def' = 10?

		float fMass; //default mass of lua-created objects

		//default inertia of lua created objects
		float fInertiaX;
		float fInertiaY;
		float fInertiaZ;

		//default physics-local position of lua created objects
		float fCursorX;
		float fCursorY;
		float fCursorZ;

};


#endif
