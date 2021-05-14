qt5_add_dbus_adaptor(DBUS_SRCS
    dbus/org.mpris.MediaPlayer2.xml
    mprisapplication.h MprisApplication)

qt5_add_dbus_adaptor(DBUS_SRCS
    dbus/org.mpris.MediaPlayer2.Player.xml
    mprisplayer.h MprisPlayer)

add_library(mpris2 STATIC mprisapplication.cpp mprisplayer.cpp ${DBUS_SRCS})
target_link_libraries(mpris2 PUBLIC Qt5::Core Qt5::DBus)
