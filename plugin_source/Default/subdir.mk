################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../AES.cpp \
../BulletPhysicsHooks.cpp \
../NASA_TurboJetSim_Solver.cpp \
../StringTools.cpp \
../TerrainMap.cpp \
../XDataRefCustom.cpp \
../XGLShader.cpp \
../XMiscUtils.cpp \
../XParticleEngine.cpp \
../XPlaneHooks.cpp \
../XTrig.cpp \
../b64.cpp \
../gizmo_httpd.cpp \
../md5.cpp \
../tgaTools.cpp \
../xlua_builtins.cpp 

C_SRCS += \
../GLee.c \
../luagl.c \
../luagl_util.c \
../luaglu.c 

OBJS += \
./AES.o \
./BulletPhysicsHooks.o \
./GLee.o \
./NASA_TurboJetSim_Solver.o \
./StringTools.o \
./TerrainMap.o \
./XDataRefCustom.o \
./XGLShader.o \
./XMiscUtils.o \
./XParticleEngine.o \
./XPlaneHooks.o \
./XTrig.o \
./b64.o \
./gizmo_httpd.o \
./luagl.o \
./luagl_util.o \
./luaglu.o \
./md5.o \
./tgaTools.o \
./xlua_builtins.o 

C_DEPS += \
./GLee.d \
./luagl.d \
./luagl_util.d \
./luaglu.d 

CPP_DEPS += \
./AES.d \
./BulletPhysicsHooks.d \
./NASA_TurboJetSim_Solver.d \
./StringTools.d \
./TerrainMap.d \
./XDataRefCustom.d \
./XGLShader.d \
./XMiscUtils.d \
./XParticleEngine.d \
./XPlaneHooks.d \
./XTrig.d \
./b64.d \
./gizmo_httpd.d \
./md5.d \
./tgaTools.d \
./xlua_builtins.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DAPL=0 -DIBM=0 -DLIN=1 -I/home/michael/CPP/SDK/CHeaders/XPLM -I/home/michael/CPP/SDK/CHeaders/Wrappers -I/home/michael/CPP/SDK/CHeaders/Widgets -I/home/michael/CPP/Gizmo-11.5.25/third_party_source -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


