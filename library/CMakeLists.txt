ADD_SUBDIRECTORY(lite)

OPTION(UIDBG "Enable or Disable uidbg" ON)
if (UIDBG)
    ADD_SUBDIRECTORY(uidbg)
ENDIF()

ADD_SUBDIRECTORY(xutility)
