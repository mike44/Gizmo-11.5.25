################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../gizmo_lua_api/api_acf.cpp \
../gizmo_lua_api/api_airframe.cpp \
../gizmo_lua_api/api_base64.cpp \
../gizmo_lua_api/api_camera.cpp \
../gizmo_lua_api/api_event_docs.cpp \
../gizmo_lua_api/api_fmc.cpp \
../gizmo_lua_api/api_gfx.cpp \
../gizmo_lua_api/api_gizmo.cpp \
../gizmo_lua_api/api_gui.cpp \
../gizmo_lua_api/api_hash.cpp \
../gizmo_lua_api/api_http.cpp \
../gizmo_lua_api/api_logging.cpp \
../gizmo_lua_api/api_matrix.cpp \
../gizmo_lua_api/api_memcache.cpp \
../gizmo_lua_api/api_menu.cpp \
../gizmo_lua_api/api_mouse.cpp \
../gizmo_lua_api/api_nav.cpp \
../gizmo_lua_api/api_particles.cpp \
../gizmo_lua_api/api_physics.cpp \
../gizmo_lua_api/api_plugins.cpp \
../gizmo_lua_api/api_shaders.cpp \
../gizmo_lua_api/api_sound.cpp \
../gizmo_lua_api/api_third_party_docs.cpp \
../gizmo_lua_api/api_timer.cpp \
../gizmo_lua_api/api_traffic.cpp \
../gizmo_lua_api/api_trig.cpp \
../gizmo_lua_api/api_utils.cpp \
../gizmo_lua_api/api_xp.cpp 

OBJS += \
./gizmo_lua_api/api_acf.o \
./gizmo_lua_api/api_airframe.o \
./gizmo_lua_api/api_base64.o \
./gizmo_lua_api/api_camera.o \
./gizmo_lua_api/api_event_docs.o \
./gizmo_lua_api/api_fmc.o \
./gizmo_lua_api/api_gfx.o \
./gizmo_lua_api/api_gizmo.o \
./gizmo_lua_api/api_gui.o \
./gizmo_lua_api/api_hash.o \
./gizmo_lua_api/api_http.o \
./gizmo_lua_api/api_logging.o \
./gizmo_lua_api/api_matrix.o \
./gizmo_lua_api/api_memcache.o \
./gizmo_lua_api/api_menu.o \
./gizmo_lua_api/api_mouse.o \
./gizmo_lua_api/api_nav.o \
./gizmo_lua_api/api_particles.o \
./gizmo_lua_api/api_physics.o \
./gizmo_lua_api/api_plugins.o \
./gizmo_lua_api/api_shaders.o \
./gizmo_lua_api/api_sound.o \
./gizmo_lua_api/api_third_party_docs.o \
./gizmo_lua_api/api_timer.o \
./gizmo_lua_api/api_traffic.o \
./gizmo_lua_api/api_trig.o \
./gizmo_lua_api/api_utils.o \
./gizmo_lua_api/api_xp.o 

CPP_DEPS += \
./gizmo_lua_api/api_acf.d \
./gizmo_lua_api/api_airframe.d \
./gizmo_lua_api/api_base64.d \
./gizmo_lua_api/api_camera.d \
./gizmo_lua_api/api_event_docs.d \
./gizmo_lua_api/api_fmc.d \
./gizmo_lua_api/api_gfx.d \
./gizmo_lua_api/api_gizmo.d \
./gizmo_lua_api/api_gui.d \
./gizmo_lua_api/api_hash.d \
./gizmo_lua_api/api_http.d \
./gizmo_lua_api/api_logging.d \
./gizmo_lua_api/api_matrix.d \
./gizmo_lua_api/api_memcache.d \
./gizmo_lua_api/api_menu.d \
./gizmo_lua_api/api_mouse.d \
./gizmo_lua_api/api_nav.d \
./gizmo_lua_api/api_particles.d \
./gizmo_lua_api/api_physics.d \
./gizmo_lua_api/api_plugins.d \
./gizmo_lua_api/api_shaders.d \
./gizmo_lua_api/api_sound.d \
./gizmo_lua_api/api_third_party_docs.d \
./gizmo_lua_api/api_timer.d \
./gizmo_lua_api/api_traffic.d \
./gizmo_lua_api/api_trig.d \
./gizmo_lua_api/api_utils.d \
./gizmo_lua_api/api_xp.d 


# Each subdirectory must supply rules for building sources it contributes
gizmo_lua_api/%.o: ../gizmo_lua_api/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DAPL=0 -DIBM=0 -DLIN=1 -I/home/michael/CPP/SDK/CHeaders/XPLM -I/home/michael/CPP/SDK/CHeaders/Wrappers -I/home/michael/CPP/SDK/CHeaders/Widgets -I/home/michael/CPP/Gizmo-11.5.25/third_party_source -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


