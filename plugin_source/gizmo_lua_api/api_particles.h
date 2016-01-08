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


int gizmo_builtin_particles_newParticles(lua_State *L);


int gizmo_builtin_particles_setBirthRate(lua_State *L);
int gizmo_builtin_particles_setBounce(lua_State *L);
int gizmo_builtin_particles_setColor(lua_State *L);
int gizmo_builtin_particles_setCount(lua_State *L);

int gizmo_builtin_particles_setDrag(lua_State *L);

int gizmo_builtin_particles_setDisabled(lua_State *L);
int gizmo_builtin_particles_setEnabled(lua_State *L);

int gizmo_builtin_particles_setEnergyMax(lua_State *L);


int gizmo_builtin_particles_setEnergyMin(lua_State *L);


int gizmo_builtin_particles_setGravity(lua_State *L);


int gizmo_builtin_particles_setHardDeck(lua_State *L);


int gizmo_builtin_particles_setLife(lua_State *L);
int gizmo_builtin_particles_setScaleRate(lua_State *L);
int gizmo_builtin_particles_setSize(lua_State *L);


int gizmo_builtin_particles_setStreamPitch(lua_State *L);
int gizmo_builtin_particles_setStreamHeading(lua_State *L);
int gizmo_builtin_particles_setStreamNoise(lua_State *L);


int gizmo_builtin_particles_setStuckToAcf(lua_State *L);
int gizmo_builtin_particles_setTexture(lua_State *L);

int gizmo_builtin_particles_setTextureSpriteCount(lua_State *L); 


int gizmo_builtin_particles_setType_Cubic(lua_State *L);
int gizmo_builtin_particles_setType_Spherical(lua_State *L);
int gizmo_builtin_particles_setType_Stream(lua_State *L);


int gizmo_builtin_particles_setOrigin(lua_State *L);
int gizmo_builtin_particles_setOrigin_Offset(lua_State *L);


int gizmo_builtin_particles_start(lua_State *L);



