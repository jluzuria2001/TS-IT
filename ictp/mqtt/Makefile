all: mqtt-demo

CONTIKI = ../../..

DEFINES+=PROJECT_CONF_H=\"../project-conf.h\"

APPS += mqtt

# Linker size optimization
SMALL = 1

# Override this due to RAM overflow
CFLAGS += -DNBR_TABLE_CONF_MAX_NEIGHBORS=3
CFLAGS += -DUIP_CONF_MAX_ROUTES=3

# This includes the httpd code from the Border Router
PROJECTDIRS += ../border-router
PROJECT_SOURCEFILES += httpd-simple.c


CONTIKI_WITH_IPV6 = 1

include $(CONTIKI)/Makefile.include
