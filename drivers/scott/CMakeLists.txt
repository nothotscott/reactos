list(APPEND SOURCE
    driver.c)

add_library(scott MODULE
    ${SOURCE}
    scott.rc)

set_module_type(scott kernelmodedriver)
add_pch(scott driver.h SOURCE)
add_importlibs(scott ntoskrnl)
add_cd_file(TARGET scott DESTINATION reactos/system32/drivers FOR all)