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


/*
 NOTE: This file is based heavily on the HelloWorld Bullet Demo that ships with Bullet 2.69.
*/

#include "GlobalDefs.h"



#include "BulletPhysicsHooks.h"

GizmoBulletHooks::GizmoBulletHooks(){

    XPLMDebugString("\t Init: Bullet 2.77 - http://www.bulletphysics.org/\n");

	this->iSimSteps = 10; //bullet rec def

	this->fCursorX = 0.f;
	this->fCursorY = 0.f;
	this->fCursorZ = 0.f;

	this->fMass = 0.f;

	this->fInertiaX = 0.f;
	this->fInertiaY = 0.f;
	this->fInertiaZ = 0.f;


	this->collisionConfiguration = new btDefaultCollisionConfiguration();
	this->dispatcher = new btCollisionDispatcher(collisionConfiguration);


	///the maximum size of the collision world. Make sure objects stay within these boundaries
	///Don't make the world AABB size too large, it will harm simulation quality and performance
	btVector3 worldAabbMin(-10000,-10000,-10000);
	btVector3 worldAabbMax(10000,10000,10000);

	//int	maxProxies = 1024;
	int	maxProxies = 16384;

	this->overlappingPairCache = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);


	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	this->solver = new btSequentialImpulseConstraintSolver;
	//this->solver = new btParallelConstraintSolver;


	this->dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,
													overlappingPairCache,
													solver,
													collisionConfiguration);

	/*
		this->dynamicsWorld = new btSoftRigidDynamicsWorld(dispatcher,
													overlappingPairCache,
													solver,
													collisionConfiguration);

	*/

	this->dynamicsWorld_ObjectArray = this->dynamicsWorld->getCollisionObjectArray(); //store a pointer so we dont have to get this repeatedly.

	this->gDebugDrawer.setDebugMode( btIDebugDraw::DBG_DrawWireframe );
	
	this->dynamicsWorld->setDebugDrawer( &this->gDebugDrawer );

	this->dynamicsWorld->setGravity(btVector3(0,-9.8,0));



}//end; init()



GizmoBulletHooks::~GizmoBulletHooks(){

    //XPLMDebugString("Gizmo: Bullet: Shut down..\n");
	//cleanup in the reverse order of creation/initialization

	this->dynamicsWorld_ObjectArray = this->dynamicsWorld->getCollisionObjectArray(); //destructor code - these updates are mandatory

	//remove the rigidbodies from the dynamics world and delete them
	int i;
	for (i=dynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		//btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btCollisionObject* obj = this->dynamicsWorld_ObjectArray[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject( obj );
		delete obj;
	}

	//delete collision shapes
	for (int j=0;j<collisionShapes.size();j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();


}//end; void cleanup()









void GizmoBulletHooks::CreateDynamicObject( btCollisionShape* colShape ){

	this->collisionShapes.push_back(colShape); //instance the object. more below to create a dynamic object out of it if need be.

	/// Create Dynamic Objects
	btScalar mass( this->fMass );

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);
	btVector3 localInertia(this->fInertiaX,this->fInertiaY,this->fInertiaZ);

	if (isDynamic){
		colShape->calculateLocalInertia(mass,localInertia);
	}


	btTransform startTransform;
	startTransform.setIdentity();

	startTransform.setOrigin(btVector3(this->fCursorX,
									   this->fCursorY,
									   this->fCursorZ));

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	this->dynamicsWorld->addRigidBody(body); //need to keep handles to these rigid body instances.

}




