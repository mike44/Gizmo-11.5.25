# Only Linux is tested. Windows or Mac should be easy to adjust accordingly though. Simply open in Qtcreator.
# Shared library without any Qt functionality

TEMPLATE = lib
QT -= gui core
DEPENDPATH += . gizmo_lua_api
INCLUDEPATH += . gizmo_lua_api

CONFIG += warn_on plugin release
CONFIG -= thread exceptions qt rtti debug

VERSION = 1.0.0

INCLUDEPATH += ../../SDK/CHeaders/XPLM
INCLUDEPATH += ../../SDK/CHeaders/Wrappers
INCLUDEPATH += ../../SDK/CHeaders/Widgets
INCLUDEPATH += /usr/local/include/bullet/


# Defined to use X-Plane SDK 2.0 capabilities - no backward compatibility before 9.0
DEFINES += XPLM200

win32 {
    DEFINES += APL=0 IBM=1 LIN=0
    LIBS += -L../SDK/Libraries/Win
    LIBS += -lXPLM -lXPWidgets
    TARGET = win.xpl
}

unix:!macx {
    DEFINES += APL=0 IBM=0 LIN=1
# HEADERS = mainwindow.h paintwidget.h
#    LIBS += -L../third_party_source/bullet-2.77/src/BulletDynamics
    LIBS += -L/usr/local/lib/libBulletDynamics.so.0

    LIBS += -llua -lBulletDynamics -lBulletCollision -lLinearMath -lOpenGLSupport -lcurl -lopenal -lalut
    TARGET = lin.xpl
    # WARNING! This requires the latest version of the X-SDK !!!!
   # QMAKE_CXXFLAGS += -fvisibility=hidden
}

macx {
    DEFINES += APL=1 IBM=0 LIN=0
    TARGET = mac.xpl
    QMAKE_LFLAGS += -flat_namespace -undefined suppress
    INCLUDEPATH += /Users/mike/workspace/Gizmo-11.5.25/third_party_source/bullet-2.77/src/
    INCLUDEPATH += /Users/mike/workspace/Gizmo-11.5.25/third_party_source/lua-5.1.4/src

    # Build for multiple architectures.
    # The following line is only needed to build universal on PPC architectures.
    # QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
    # The following line defines for which architectures we build.
    CONFIG += x86
}

HEADERS += AES.h \
           api_namespace_tables.h \
           b64.h \
           BulletPhysicsHooks.h \
           gfw_IO_Jail.h \
           gizmo_global_datarefs.h \
           gizmo_httpd.h \
           GLee.h \
           GlobalDefs.h \
           luagl.h \
           luagl_util.h \
           luaglu.h \
           md5.h \
           NASA_TurboJetSim_Solver.h \
           OpenALHooks.h \
           StringTools.h \
           TerrainMap.h \
           tgaTools.h \
           X_STL.h \
           XAC3DLoader.h \
           XDataRefCustom.h \
           XGLShader.h \
           xlua_builtins.h \
           xlua_utils_CurlInterchange.h \
           xlua_utils_FboWrapper.h \
           XLuaCustomCommandWrapper.h \
           XLuaWidgetBaseWrapper.h \
           XLuaWidgetWindowWrapper.h \
           XMiscUtils.h \
           XParticle.h \
           XParticleEngine.h \
           XPlaneHooks.h \
           XTrig.h \
           gizmo_lua_api/api_acf.h \
           gizmo_lua_api/api_airframe.h \
           gizmo_lua_api/api_base64.h \
           gizmo_lua_api/api_camera.h \
           gizmo_lua_api/api_fmc.h \
           gizmo_lua_api/api_gfx.h \
           gizmo_lua_api/api_gizmo.h \
           gizmo_lua_api/api_gui.h \
           gizmo_lua_api/api_hash.h \
           gizmo_lua_api/api_http.h \
           gizmo_lua_api/api_logging.h \
           gizmo_lua_api/api_matrix.h \
           gizmo_lua_api/api_memcache.h \
           gizmo_lua_api/api_menu.h \
           gizmo_lua_api/api_mouse.h \
           gizmo_lua_api/api_nav.h \
           gizmo_lua_api/api_particles.h \
           gizmo_lua_api/api_physics.h \
           gizmo_lua_api/api_plugins.h \
           gizmo_lua_api/api_shaders.h \
           gizmo_lua_api/api_sound.h \
           gizmo_lua_api/api_timer.h \
           gizmo_lua_api/api_traffic.h \
           gizmo_lua_api/api_trig.h \
           gizmo_lua_api/api_utils.h \
           gizmo_lua_api/api_xp.h \
           gizmo_lua_api/api_acf.cpp \
           gizmo_lua_api/api_base64.cpp \
           gizmo_lua_api/api_camera.cpp \
           gizmo_lua_api/api_fmc.cpp \
           gizmo_lua_api/api_gizmo.cpp \
           gizmo_lua_api/api_gfx.cpp \
           gizmo_lua_api/api_gui.cpp \
           gizmo_lua_api/api_hash.cpp \
           gizmo_lua_api/api_http.cpp \
           gizmo_lua_api/api_logging.cpp \
           gizmo_lua_api/api_matrix.cpp \
           gizmo_lua_api/api_memcache.cpp \
           gizmo_lua_api/api_menu.cpp \
           gizmo_lua_api/api_mouse.cpp \
           gizmo_lua_api/api_nav.cpp \
           gizmo_lua_api/api_particles.cpp \
           gizmo_lua_api/api_physics.cpp \
           gizmo_lua_api/api_plugins.cpp \
           gizmo_lua_api/api_shaders.cpp \
           gizmo_lua_api/api_sound.cpp \
           gizmo_lua_api/api_timer.cpp \
           gizmo_lua_api/api_traffic.cpp \
           gizmo_lua_api/api_trig.cpp \
           gizmo_lua_api/api_utils.cpp \
           gizmo_lua_api/api_xp.cpp \
           Gizmo_Logo.h_data \
           xa_logo.h_data \
           lua_logo.h_data \
           ToolTrayIconsBuiltin.h_data \
           XGLShader.cpp \
           xlua_builtins.cpp
SOURCES += AES.cpp \
           b64.cpp \
           BulletPhysicsHooks.cpp \
           gizmo_httpd.cpp \
           GLee.c \
           luagl.c \
           luagl_util.c \
           luaglu.c \
           md5.cpp \
           NASA_TurboJetSim_Solver.cpp \
           StringTools.cpp \
           TerrainMap.cpp \
           tgaTools.cpp \
           XDataRefCustom.cpp \
           XGLShader.cpp \
           xlua_builtins.cpp \
           XMiscUtils.cpp \
           XParticleEngine.cpp \
           XPlaneHooks.cpp \
           XTrig.cpp \
           gizmo_lua_api/api_acf.cpp \
           gizmo_lua_api/api_airframe.cpp \
           gizmo_lua_api/api_base64.cpp \
           gizmo_lua_api/api_camera.cpp \
           gizmo_lua_api/api_event_docs.cpp \
           gizmo_lua_api/api_fmc.cpp \
           gizmo_lua_api/api_gfx.cpp \
           gizmo_lua_api/api_gizmo.cpp \
           gizmo_lua_api/api_gui.cpp \
           gizmo_lua_api/api_hash.cpp \
           gizmo_lua_api/api_http.cpp \
           gizmo_lua_api/api_logging.cpp \
           gizmo_lua_api/api_matrix.cpp \
           gizmo_lua_api/api_memcache.cpp \
           gizmo_lua_api/api_menu.cpp \
           gizmo_lua_api/api_mouse.cpp \
           gizmo_lua_api/api_nav.cpp \
           gizmo_lua_api/api_particles.cpp \
           gizmo_lua_api/api_physics.cpp \
           gizmo_lua_api/api_plugins.cpp \
           gizmo_lua_api/api_shaders.cpp \
           gizmo_lua_api/api_sound.cpp \
           gizmo_lua_api/api_third_party_docs.cpp \
           gizmo_lua_api/api_timer.cpp \
           gizmo_lua_api/api_traffic.cpp \
           gizmo_lua_api/api_trig.cpp \
           gizmo_lua_api/api_utils.cpp \
           gizmo_lua_api/api_xp.cpp
