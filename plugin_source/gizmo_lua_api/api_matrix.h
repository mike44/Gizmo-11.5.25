/*
 *  api_matrix.h
 *  Gizmo
 *
 *  Created by Ben Russell on 20/03/11.
 *  Copyright 2011 X-Plugins.com. All rights reserved.
 *
 */




int gizmo_builtin_matrix_transform3D(lua_State *L);




void gizmo_Transform3D(
				 const double inLocalCoords[3],	// acf coordinates
				 const double inMap[6],			// x, y, z, heading, pitch, roll (X-Plane has angles in degrees)
				 double       outCoords[3]
				 );

void gizmo_mmult(const double inX[3], const double R[3][3], double outX[3]);


