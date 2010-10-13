# Microsoft Developer Studio Project File - Name="chuck_win32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=chuck_win32 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "chuck_win32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "chuck_win32.mak" CFG="chuck_win32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "chuck_win32 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "chuck_win32 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /D "NDEBUG" /D "__PLATFORM_WIN32__" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__WINDOWS_DS__" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dsound.lib dinput.lib dxguid.lib wsock32.lib winmm.lib /nologo /subsystem:console /machine:I386 /out:"Release/chuck.exe"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /D "_DEBUG" /D "__PLATFORM_WIN32__" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "__WINDOWS_DS__" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dinput.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dsound.lib dxguid.lib wsock32.lib winmm.lib /nologo /subsystem:console /debug /machine:I386 /out:"Debug/chuck.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "chuck_win32 - Win32 Release"
# Name "chuck_win32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\chuck_absyn.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_bbq.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_compile.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_console.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_dl.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_emit.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_errmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_frame.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_globals.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_instr.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_lang.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_main.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_oo.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_otf.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_parse.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_scan.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_shell.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_stats.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_symbol.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_table.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_type.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_ugen.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_utils.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_vm.cpp
# End Source File
# Begin Source File

SOURCE=.\chuck_win32.c
# End Source File
# Begin Source File

SOURCE=.\digiio_rtaudio.cpp
# End Source File
# Begin Source File

SOURCE=.\hidio_sdl.cpp
# End Source File
# Begin Source File

SOURCE=.\midiio_rtmidi.cpp
# End Source File
# Begin Source File

SOURCE=.\rtaudio.cpp
# End Source File
# Begin Source File

SOURCE=.\rtmidi.cpp
# End Source File
# Begin Source File

SOURCE=.\uana_extract.cpp
# End Source File
# Begin Source File

SOURCE=.\uana_xform.cpp
# End Source File
# Begin Source File

SOURCE=.\ugen_filter.cpp
# End Source File
# Begin Source File

SOURCE=.\ugen_osc.cpp
# End Source File
# Begin Source File

SOURCE=.\ugen_stk.cpp
# End Source File
# Begin Source File

SOURCE=.\ugen_xxx.cpp
# End Source File
# Begin Source File

SOURCE=.\ulib_machine.cpp
# End Source File
# Begin Source File

SOURCE=.\ulib_math.cpp
# End Source File
# Begin Source File

SOURCE=.\ulib_opsc.cpp
# End Source File
# Begin Source File

SOURCE=.\ulib_std.cpp
# End Source File
# Begin Source File

SOURCE=.\util_buffers.cpp
# End Source File
# Begin Source File

SOURCE=.\util_console.cpp
# End Source File
# Begin Source File

SOURCE=.\util_hid.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

# ADD CPP /GX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_math.c
# End Source File
# Begin Source File

SOURCE=.\util_network.c
# End Source File
# Begin Source File

SOURCE=.\util_opsc.cpp
# End Source File
# Begin Source File

SOURCE=.\util_raw.c
# End Source File
# Begin Source File

SOURCE=.\util_sndfile.c
# End Source File
# Begin Source File

SOURCE=.\util_string.cpp
# End Source File
# Begin Source File

SOURCE=.\util_thread.cpp
# End Source File
# Begin Source File

SOURCE=.\util_xforms.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\chuck_absyn.h
# End Source File
# Begin Source File

SOURCE=.\chuck_bbq.h
# End Source File
# Begin Source File

SOURCE=.\chuck_compile.h
# End Source File
# Begin Source File

SOURCE=.\chuck_console.h
# End Source File
# Begin Source File

SOURCE=.\chuck_def.h
# End Source File
# Begin Source File

SOURCE=.\chuck_dl.h
# End Source File
# Begin Source File

SOURCE=.\chuck_emit.h
# End Source File
# Begin Source File

SOURCE=.\chuck_errmsg.h
# End Source File
# Begin Source File

SOURCE=.\chuck_frame.h
# End Source File
# Begin Source File

SOURCE=.\chuck_globals.h
# End Source File
# Begin Source File

SOURCE=.\chuck_instr.h
# End Source File
# Begin Source File

SOURCE=.\chuck_lang.h
# End Source File
# Begin Source File

SOURCE=.\chuck_oo.h
# End Source File
# Begin Source File

SOURCE=.\chuck_otf.h
# End Source File
# Begin Source File

SOURCE=.\chuck_parse.h
# End Source File
# Begin Source File

SOURCE=.\chuck_scan.h
# End Source File
# Begin Source File

SOURCE=.\chuck_shell.h
# End Source File
# Begin Source File

SOURCE=.\chuck_stats.h
# End Source File
# Begin Source File

SOURCE=.\chuck_symbol.h
# End Source File
# Begin Source File

SOURCE=.\chuck_table.h
# End Source File
# Begin Source File

SOURCE=.\chuck_type.h
# End Source File
# Begin Source File

SOURCE=.\chuck_ugen.h
# End Source File
# Begin Source File

SOURCE=.\chuck_utils.h
# End Source File
# Begin Source File

SOURCE=.\chuck_vm.h
# End Source File
# Begin Source File

SOURCE=.\chuck_win32.h
# End Source File
# Begin Source File

SOURCE=.\digiio_rtaudio.h
# End Source File
# Begin Source File

SOURCE=.\dirent.h
# End Source File
# Begin Source File

SOURCE=.\hidio_sdl.h
# End Source File
# Begin Source File

SOURCE=.\midiio_rtmidi.h
# End Source File
# Begin Source File

SOURCE=.\rtaudio.h
# End Source File
# Begin Source File

SOURCE=.\rterror.h
# End Source File
# Begin Source File

SOURCE=.\rtmidi.h
# End Source File
# Begin Source File

SOURCE=.\uana_extract.h
# End Source File
# Begin Source File

SOURCE=.\uana_xform.h
# End Source File
# Begin Source File

SOURCE=.\ugen_filter.h
# End Source File
# Begin Source File

SOURCE=.\ugen_osc.h
# End Source File
# Begin Source File

SOURCE=.\ugen_stk.h
# End Source File
# Begin Source File

SOURCE=.\ugen_xxx.h
# End Source File
# Begin Source File

SOURCE=.\ulib_machine.h
# End Source File
# Begin Source File

SOURCE=.\ulib_math.h
# End Source File
# Begin Source File

SOURCE=.\ulib_opsc.h
# End Source File
# Begin Source File

SOURCE=.\ulib_std.h
# End Source File
# Begin Source File

SOURCE=.\util_buffers.h
# End Source File
# Begin Source File

SOURCE=.\util_console.h
# End Source File
# Begin Source File

SOURCE=.\util_hid.h
# End Source File
# Begin Source File

SOURCE=.\util_math.h
# End Source File
# Begin Source File

SOURCE=.\util_network.h
# End Source File
# Begin Source File

SOURCE=.\util_opsc.h
# End Source File
# Begin Source File

SOURCE=.\util_raw.h
# End Source File
# Begin Source File

SOURCE=.\util_sndfile.h
# End Source File
# Begin Source File

SOURCE=.\util_string.h
# End Source File
# Begin Source File

SOURCE=.\util_thread.h
# End Source File
# Begin Source File

SOURCE=.\util_xforms.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
