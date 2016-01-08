# Linux Makefile. Not tested, as I prefer to use standard distro libs and open the gizmo.pro file in Qtcreator.

BULLET_BASE=../third_party_source/bullet-2.77
BULLET_PHYSICS_SRC=$(BULLET_BASE)/src
BULLET_LIBS=$(BULLET_BASE)/libs_br
BULLET_SUPPORT=$(BULLET_BASE)/Demos/OpenGL

XPLANE_SDK=../third_party_source/X-Plane-SDK-201/CHeaders

#Standard LuaVM source, no JIT.
NOJ_LUA_SRC=../third_party_source/lua-5.1.4/src

#LuaJIT source.
JIT_LUA_SRC=../third_party_source/LuaJIT-1.1.6/src
DYN_ASM_SRC=../third_party_source/LuaJIT-1.1.6/dynasm

#Source to use during build.
LUA_SRC=$(JIT_LUA_SRC)



# Not used.
#STATIC_CURL=../third_party_source/curl-7.19.7/include



XPLANE_PLUGINS_FOLDER="/home/br/x-plane/X-Plane 9 Demo/Resources/plugins/"


#C++ flags for compiler
CFLAGS=-march=pentium4 -mtune=pentium4 -W -g -DIBM=0 -DAPL=0 -DLIN=1 -DXPLM200=1\
		-I$(XPLANE_SDK)/Widgets \
		-I$(XPLANE_SDK)/Wrappers \
		-I$(XPLANE_SDK)/XPLM \
		-I$(LUA_SRC) \
		-I$(BULLET_PHYSICS_SRC) \
		-I$(BULLET_SUPPORT)

CXXFLAGS=$(CFLAGS)


#lib flags for linker
LFLAGS= -L$(BULLET_LIBS) \
	-L$(LUA_SRC) \
		-llua \
		-lBulletDynamics \
		-lBulletCollision \
		-lLinearMath \
		-lcurl \
		-lopenal \
		-lalut \
		#-lBulletOpenglsupport \
		#-ldmalloccxx



OBJS=\
	XDataRefCustom.o \
	luagl.o \
	luagl_util.o \
	tgaTools.o \
	XTrig.o \
	GLee.o \
	XParticleEngine.o \
	b64.o \
	gizmo_httpd.o \
	StringTools.o \
	XPlaneHooks.o \
	BulletPhysicsHooks.o \
	TerrainMap.o \
	luaglu.o


HEADERS=\
	OpenALHooks.h \
	XParticle.h \
	xlua_utils_FboWrapper.h \


OUT_XPL=Gizmo_Linux.xpl



Gizmo: $(OBJS)
	@echo Building..
	g++ -shared -o $(OUT_XPL) $(OBJS) $(HEADERS) $(CXXFLAGS) $(LFLAGS) 
	@echo
	chmod +x $(OUT_XPL)	
	@echo Done!

clean:
	@-rm *.o
	@-rm *.so
	@-rm *.xpl
	@echo Done.

install:
	cp $(OUT_XPL) $(XPLANE_PLUGINS_FOLDER)







