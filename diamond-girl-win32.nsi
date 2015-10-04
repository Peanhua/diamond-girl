  !define APPNAME "Lucy the Diamond Girl"
  !define COMPANYNAME "Joni Yrjana"
  !define DESCRIPTION "Single player puzzle game with a time constraint"
  !define VERSIONMAJOR x
  !define VERSIONMINOR y
  !define VERSIONPATCH z
  !define HELPURL   "http://www.netikka.net/joyr/diamond_girl/"
  !define UPDATEURL "${HELPURL}"
  !define ABOUTURL  "${HELPURL}"
  !define INSTALLSIZE 0

  !include "MUI2.nsh"

  Name "${APPNAME}"
  Icon "diamond-girl.ico"
  OutFile "diamond-girl.exe"
  InstallDir "$PROGRAMFILES\Lucy the Diamond Girl"

; Request application privileges for Windows Vista
  RequestExecutionLevel admin

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "themes\default\gfx\logo-win32installer.bmp"

  !define MUI_COMPONENTSPAGE_NODESC

;--------------------------------

; Pages

  !insertmacro MUI_PAGE_LICENSE "LICENSE"

  Var StartMenuFolder
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Diamond_Girl" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  !insertmacro MUI_PAGE_STARTMENU "Application" $StartMenuFolder

  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

  !insertmacro MUI_LANGUAGE "English"

;--------------------------------

; The stuff to install
Section "!Lucy the Diamond Girl"

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  File /r *.*
  
  ; Write the installation path into the registry
  WriteRegStr HKLM "Software\Diamond_Girl" "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "Lucy the Diamond Girl"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayIcon"   "$INSTDIR\diamond-girl.ico"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "Publisher"     "${COMPANYNAME}"
  ; WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "HelpLink"      "${HELPURL}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "URLUpdateInfo" "${UPDATEURL}"
  ; WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "URLInfoAbout"   "${ABOUTURL}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayVersion" "${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONPATCH}"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "VersionMajor" ${VERSIONMAJOR}
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "VersionMinor" ${VERSIONMINOR}
  # Set the INSTALLSIZE constant (!defined at the top of this script) so Add/Remove Programs can accurately report the size
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "EstimatedSize" ${INSTALLSIZE}

  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

  ; Start menu integration:
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Lucy the Diamond Girl.lnk" "$INSTDIR\diamond-girl.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd


;--------------------------------

; Uninstaller

Section "Uninstall"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\Lucy the Diamond Girl.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
  DeleteRegKey HKLM "Software\Diamond_Girl"

  ; Remove installed files
  Delete   "$INSTDIR\Changelog"
  Delete   "$INSTDIR\CREDITS"
  RMDir /r "$INSTDIR\data"
  Delete   "$INSTDIR\diamond-girl.exe"
  Delete   "$INSTDIR\diamond-girl.ico"
  Delete   "$INSTDIR\LICENSE"
  RMDir /r "$INSTDIR\locale"
  RMDir /r "$INSTDIR\maps"
  Delete   "$INSTDIR\README"
  RMDir /r "$INSTDIR\themes"
  Delete   "$INSTDIR\uninstall.exe"
  RMDir /r "$INSTDIR\ui"
  ; Libraries:
  Delete   "$INSTDIR\OpenAL32.dll"
  Delete   "$INSTDIR\SDL.dll"
  Delete   "$INSTDIR\SDL_image.dll"
  Delete   "$INSTDIR\glew32.dll"
  Delete   "$INSTDIR\iconv.dll"
  Delete   "$INSTDIR\libassimp.dll"
  Delete   "$INSTDIR\libcrypto-10.dll"
  Delete   "$INSTDIR\libFLAC-8.dll"
  Delete   "$INSTDIR\libSDL_gfx-15.dll"
  Delete   "$INSTDIR\libbz2-1.dll"
  Delete   "$INSTDIR\libgcc_s_sjlj-1.dll"
  Delete   "$INSTDIR\libintl-8.dll"
  Delete   "$INSTDIR\libjpeg-62.dll"
  Delete   "$INSTDIR\libjson-c-2.dll"
  Delete   "$INSTDIR\libmikmod-3.dll"
  Delete   "$INSTDIR\libogg-0.dll"
  Delete   "$INSTDIR\libpng16-16.dll"
  Delete   "$INSTDIR\libsndfile-1.dll"
  Delete   "$INSTDIR\libstdc++-6.dll"
  Delete   "$INSTDIR\libtiff-5.dll"
  Delete   "$INSTDIR\libvorbis-0.dll"
  Delete   "$INSTDIR\libvorbisenc-2.dll"
  Delete   "$INSTDIR\libvorbisfile-3.dll"
  Delete   "$INSTDIR\libwinpthread-1.dll"
  Delete   "$INSTDIR\zlib1.dll"

  RMDir "$INSTDIR"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  
SectionEnd
