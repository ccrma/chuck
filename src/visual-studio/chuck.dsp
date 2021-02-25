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

# PROP Default_Filter "cc;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\chuck_absyn.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

#SOURCE=.\chuck_bbq.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_compile.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_console.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_dl.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_emit.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_errmsg.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_frame.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_globals.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_instr.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_io.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_lang.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_main.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_oo.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_otf.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_parse.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_scan.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_shell.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_stats.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_symbol.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_table.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_type.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_ugen.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_utils.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\chuck_vm.cc

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

SOURCE=.\digiio_rtaudio.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\hidio_sdl.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\midiio_rtmidi.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rtaudio.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\rtmidi.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uana_extract.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\uana_xform.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ugen_filter.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ugen_osc.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ugen_stk.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ugen_xxx.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ulib_machine.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ulib_math.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ulib_opsc.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ulib_regex.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ulib_std.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_buffers.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_console.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_hid.cc

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

SOURCE=.\util_opsc.cc

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

SOURCE=.\util_serial.cc

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

SOURCE=.\util_string.cc

!IF  "$(CFG)" == "chuck_win32 - Win32 Release"

# ADD CPP /D "HAVE_CONFIG_H"

!ELSEIF  "$(CFG)" == "chuck_win32 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\util_thread.cc

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

# PROP Default_Filter "h;hh;hxx;hm;inl"
# Begin Source File

SOURCE=.\chuck_absyn.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_bbq.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_compile.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_console.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_def.h
# End Source File
# Begin Source File

SOURCE=.\chuck_dl.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_emit.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_errmsg.h
# End Source File
# Begin Source File

SOURCE=.\chuck_frame.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_globals.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_instr.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_io.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_lang.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_oo.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_otf.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_parse.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_scan.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_shell.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_stats.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_symbol.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_table.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_type.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_ugen.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_utils.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_vm.hh
# End Source File
# Begin Source File

SOURCE=.\chuck_win32.h
# End Source File
# Begin Source File

SOURCE=.\digiio_rtaudio.hh
# End Source File
# Begin Source File

SOURCE=.\dirent.h
# End Source File
# Begin Source File

SOURCE=.\hidio_sdl.hh
# End Source File
# Begin Source File

SOURCE=.\midiio_rtmidi.hh
# End Source File
# Begin Source File

SOURCE=.\rtaudio.hh
# End Source File
# Begin Source File

SOURCE=.\rtmidi.hh
# End Source File
# Begin Source File

SOURCE=.\uana_extract.hh
# End Source File
# Begin Source File

SOURCE=.\uana_xform.hh
# End Source File
# Begin Source File

SOURCE=.\ugen_filter.hh
# End Source File
# Begin Source File

SOURCE=.\ugen_osc.hh
# End Source File
# Begin Source File

SOURCE=.\ugen_stk.hh
# End Source File
# Begin Source File

SOURCE=.\ugen_xxx.hh
# End Source File
# Begin Source File

SOURCE=.\ulib_machine.hh
# End Source File
# Begin Source File

SOURCE=.\ulib_math.hh
# End Source File
# Begin Source File

SOURCE=.\ulib_opsc.hh
# End Source File
# Begin Source File

SOURCE=.\ulib_regex.hh
# End Source File
# Begin Source File

SOURCE=.\ulib_std.hh
# End Source File
# Begin Source File

SOURCE=.\util_buffers.hh
# End Source File
# Begin Source File

SOURCE=.\util_console.hh
# End Source File
# Begin Source File

SOURCE=.\util_hid.hh
# End Source File
# Begin Source File

SOURCE=.\util_math.h
# End Source File
# Begin Source File

SOURCE=.\util_network.h
# End Source File
# Begin Source File

SOURCE=.\util_opsc.hh
# End Source File
# Begin Source File

SOURCE=.\util_raw.h
# End Source File
# Begin Source File

SOURCE=.\util_serial.hh
# End Source File
# Begin Source File

SOURCE=.\util_sndfile.h
# End Source File
# Begin Source File

SOURCE=.\util_string.hh
# End Source File
# Begin Source File

SOURCE=.\util_thread.hh
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
