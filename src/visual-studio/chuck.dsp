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

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_bbq.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_compile.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_console.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_dl.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_emit.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_errmsg.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_frame.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_globals.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_instr.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_io.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_lang.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_main.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_oo.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_otf.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_parse.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_scan.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_shell.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_stats.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_symbol.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_table.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_type.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_ugen.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_utils.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_vm.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_win32.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\digiio_rtaudio.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hidio_sdl.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\midiio_rtmidi.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RtAudio\RtAudio.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rtmidi.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uana_extract.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uana_xform.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ugen_filter.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ugen_osc.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ugen_stk.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ugen_xxx.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ulib_machine.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ulib_math.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ulib_opsc.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ulib_regex.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ulib_std.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_buffers.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_console.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_hid.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

# ADD CPP /GX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_math.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_network.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_opsc.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_raw.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_serial.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_sndfile.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_string.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_thread.cpp

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_xforms.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

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

SOURCE=.\chuck_io.h
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

SOURCE=.\RtAudio\RtAudio.h
# End Source File
# Begin Source File

SOURCE=.\RtAudio\RtError.h
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

SOURCE=.\ulib_regex.h
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

SOURCE=.\util_serial.h
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
# Begin Group "regex"

# PROP Default_Filter ""
# Begin Group "regex source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\regex\regcomp.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "regex" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\regex\regerror.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "regex" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\regex\regexec.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "regex" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\regex\tre-ast.c"

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "regex" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\regex\tre-compile.c"

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "regex" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\regex\tre-filter.c"

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "regex" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\regex\tre-match-approx.c"

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "regex" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\regex\tre-match-backtrack.c"

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "regex" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\regex\tre-match-parallel.c"

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "regex" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\regex\tre-mem.c"

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "regex" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\regex\tre-parse.c"

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "regex" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\regex\tre-stack.c"

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "regex" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\regex\xmalloc.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "regex" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "regex include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\regex\config.h
# End Source File
# Begin Source File

SOURCE=.\regex\regex.h
# End Source File
# Begin Source File

SOURCE=".\regex\tre-ast.h"
# End Source File
# Begin Source File

SOURCE=".\regex\tre-compile.h"
# End Source File
# Begin Source File

SOURCE=".\regex\tre-config.h"
# End Source File
# Begin Source File

SOURCE=".\regex\tre-filter.h"
# End Source File
# Begin Source File

SOURCE=".\regex\tre-internal.h"
# End Source File
# Begin Source File

SOURCE=".\regex\tre-match-utils.h"
# End Source File
# Begin Source File

SOURCE=".\regex\tre-mem.h"
# End Source File
# Begin Source File

SOURCE=".\regex\tre-parse.h"
# End Source File
# Begin Source File

SOURCE=".\regex\tre-stack.h"
# End Source File
# Begin Source File

SOURCE=.\regex\tre.h
# End Source File
# Begin Source File

SOURCE=.\regex\xmalloc.h
# End Source File
# End Group
# End Group
# Begin Group "lo"

# PROP Default_Filter ""
# Begin Group "lo source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lo\address.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "./" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lo\blob.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "./" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lo\bundle.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "./" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lo\message.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "./" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lo\method.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "./" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lo\pattern_match.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "./" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lo\send.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "./" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lo\server.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "./" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\lo\timetag.c

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /I "./" /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "lo include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\lo\config.h
# End Source File
# Begin Source File

SOURCE=.\lo\lo.h
# End Source File
# Begin Source File

SOURCE=.\lo\lo_endian.h
# End Source File
# Begin Source File

SOURCE=.\lo\lo_errors.h
# End Source File
# Begin Source File

SOURCE=.\lo\lo_internal.h
# End Source File
# Begin Source File

SOURCE=.\lo\lo_lowlevel.h
# End Source File
# Begin Source File

SOURCE=.\lo\lo_macros.h
# End Source File
# Begin Source File

SOURCE=.\lo\lo_osc_types.h
# End Source File
# Begin Source File

SOURCE=.\lo\lo_throw.h
# End Source File
# Begin Source File

SOURCE=.\lo\lo_types.h
# End Source File
# Begin Source File

SOURCE=.\lo\lo_types_internal.h
# End Source File
# Begin Source File

SOURCE=.\lo\stdint.h
# End Source File
# End Group
# End Group
# End Target
# End Project
