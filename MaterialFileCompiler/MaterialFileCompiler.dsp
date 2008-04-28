# Microsoft Developer Studio Project File - Name="MaterialFileCompiler" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MaterialFileCompiler - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "MaterialFileCompiler.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "MaterialFileCompiler.mak" CFG="MaterialFileCompiler - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "MaterialFileCompiler - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "MaterialFileCompiler - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MaterialFileCompiler - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "MaterialFileCompiler - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\App2\MaterialFileCompiler"
# PROP Intermediate_Dir "..\..\X_Debug\MaterialFileCompiler"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3d9.lib d3dx9.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "MaterialFileCompiler - Win32 Release"
# Name "MaterialFileCompiler - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\MaterialCompiler_Main.cpp
# End Source File
# Begin Source File

SOURCE=.\MaterialFileCompiler.cpp
# End Source File
# Begin Source File

SOURCE=.\MaterialFileCompiler.h
# End Source File
# Begin Source File

SOURCE=..\..\Project1\BSPStageCommon\SurfaceMaterialData.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Project1\BSPStageCommon\SurfaceMaterialData.h
# End Source File
# End Group
# Begin Group "Support"

# PROP Default_Filter ""
# Begin Group "Serialization"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Project1\Support\Serialization\Archive.h
# End Source File
# Begin Source File

SOURCE=..\..\Project1\Support\Serialization\ArchiveObjectBase.h
# End Source File
# Begin Source File

SOURCE=..\..\Project1\Support\Serialization\BinaryArchive_Input.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Project1\Support\Serialization\BinaryArchive_Input.h
# End Source File
# Begin Source File

SOURCE=..\..\Project1\Support\Serialization\BinaryArchive_Output.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Project1\Support\Serialization\BinaryArchive_Output.h
# End Source File
# Begin Source File

SOURCE=..\..\Project1\Support\Serialization\Serialization.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\Project1\Support\FilenameOperation.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Project1\Support\FilenameOperation.h
# End Source File
# Begin Source File

SOURCE=..\..\Project1\Support\FixedVector.h
# End Source File
# Begin Source File

SOURCE=..\..\Project1\Support\StringFileIO.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Project1\Support\StringFileIO.h
# End Source File
# Begin Source File

SOURCE=..\..\Project1\Support\StringList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Project1\Support\StringList.h
# End Source File
# End Group
# Begin Group "reference"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Project1\BSPStageCommon\BaseEntityHandle.h

!IF  "$(CFG)" == "MaterialFileCompiler - Win32 Release"

!ELSEIF  "$(CFG)" == "MaterialFileCompiler - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Project1\BSPStageCommon\SurfaceMaterial.cpp

!IF  "$(CFG)" == "MaterialFileCompiler - Win32 Release"

!ELSEIF  "$(CFG)" == "MaterialFileCompiler - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\Project1\BSPStageCommon\SurfaceMaterial.h

!IF  "$(CFG)" == "MaterialFileCompiler - Win32 Release"

!ELSEIF  "$(CFG)" == "MaterialFileCompiler - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "3DCommon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Project1\3DCommon\CDirectX9.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Project1\3DCommon\CDirectX9.h
# End Source File
# Begin Source File

SOURCE=..\..\Project1\3DCommon\Font.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Project1\3DCommon\Font.h
# End Source File
# End Group
# End Target
# End Project
