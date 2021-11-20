
list(APPEND SOURCE
     main.c)
    
add_executable(scottapp ${SOURCE} resource.rc)
set_module_type(scottapp win32cui)
add_importlibs(scottapp msvcrt kernel32 advapi32)
add_cd_file(TARGET scottapp DESTINATION reactos/system32 FOR all)
