ChucK for Microsoft Visual Studio ( VC6.0 , and C++ 2004 express edition ) 


Getting Started:

1.  checkout chuck from cvs
	- move the "chuck_win32.*" files from 'exile' to the 'src' directory

2. You will need to download some additional libraries and utilities 
before compiling ChucK

	yacc parsing : 
	
	- bison and flex ( bison.hairy, bison.simple ) 
	- http://www.monmouth.com/~wstreett/lex-yacc/lex-yacc.html
	- add bison.hairy and bison.simple to the /src directory.
	- place bison.exe and flex.exe into your application directory
		and make sure that Vis Studio will search that directory
		VIS STUDIO 6.0 	-- ( options -> directories : select 'for executable files' )
		VIS C++ EXPRESS  -- ( options -> Projects and Solutions -> VC++ Directories -> select 'for executable files' 	
	

INSTRUCTIONS FOR VISUAL STUDIO 6:
	-all necessary libraries should be included in your installation of vis studio 6.0

3. open the "chuck.dsw" workspace.
4. select either "debug" or "release" configuration
5. build ( F7 key )
	- after compiling, the executable will be copied to the current directory
	
	
INSTRUCTIONS FOR VISUAL STUDIO 2005 C++ EXPRESS EDITION - (currently beta - Whidbey )
	microsoft has made vis studio 2005 available as a free download.
	- download and install from their website - http://lab.msdn.microsoft.com/vs2005/

3.	install necessary libraries and SDK's
	- Microsoft SDK ( Core, MDAC ) 	
		-	http://www.microsoft.com/msdownload/platformsdk/sdkupdate/
		-	install both the Core and MDAC sections of the SDK
		-	should be place in C:/Program Files/Microsoft SDK/
	  
	- Microsoft DirectX SDK 
	  	-	http://www.msdn.microsoft.com/library/default.asp?url=/downloads/list/directx.asp	 
		-	Project assumes install location is C:/Program Files/Microsoft DirectX 9.0 SDK/

4.	open the "chuck.sln" solution file.
5.	Select configuration version	 
6.  Build!
	- after compiling, the executable will be copied to the current directory
