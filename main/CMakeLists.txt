# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.c gpio.c web_server.c wifi.c restart.c")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
