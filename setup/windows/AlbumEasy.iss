;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;Inno Setup installation script for AlbumEasy
;(c) 2005-2013 Clive Levinson
; Bundu Technology Ltd.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[Setup]
AppName=AlbumEasy

AppVerName={#AppVersionName}
AppPublisher=AlbumEasy

SourceDir=../..
             ;output directory, the same as the source directory
OutputDir=.

LicenseFile=licence.txt
SetupIconFile=setup/windows/install.ico

UsePreviousAppDir=no
DefaultDirName={pf}\AlbumEasy
DefaultGroupName=AlbumEasy
Compression=lzma
SolidCompression=yes




[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons};


[Files]
Source: AlbumEasy.exe;                       DestDir: {app};              Flags: ignoreversion
Source: licence.txt;                         DestDir: {app};              Flags: ignoreversion
Source: setup/VERSION;                       DestDir: {app};              Flags: ignoreversion

Source: fonts/DejaVuSans.ttf;                DestDir: {app}/fonts/;       Flags: ignoreversion
Source: fonts/DejaVuSerif.ttf;               DestDir: {app}/fonts/;       Flags: ignoreversion
Source: fonts/PenguinAttack.ttf;             DestDir: {app}/fonts/;       Flags: ignoreversion

Source: help/codepages.html;                 DestDir: {app}/help/;        Flags: ignoreversion
Source: help/commands.html;                  DestDir: {app}/help/;        Flags: ignoreversion
Source: help/config.html;                    DestDir: {app}/help/;        Flags: ignoreversion
Source: help/examples.html;                  DestDir: {app}/help/;        Flags: ignoreversion
Source: help/fonts.html;                     DestDir: {app}/help/;        Flags: ignoreversion
Source: help/getstarted.html;                DestDir: {app}/help/;        Flags: ignoreversion
Source: help/index.html;                     DestDir: {app}/help/;        Flags: ignoreversion
Source: help/licence.html;                   DestDir: {app}/help/;        Flags: ignoreversion

Source: help/images/chkDeselectedBullet.png;   DestDir: {app}/help/images/; Flags: ignoreversion
Source: help/images/chkSelectedBullet.png;     DestDir: {app}/help/images/; Flags: ignoreversion
Source: help/images/cmnds_page_start_var.png;  DestDir: {app}/help/images/; Flags: ignoreversion
Source: help/images/cmnds_row_alignment.png;   DestDir: {app}/help/images/; Flags: ignoreversion
Source: help/images/cmnds_stamp_add_blank.png; DestDir: {app}/help/images/; Flags: ignoreversion
Source: help/images/code_table.png;            DestDir: {app}/help/images/; Flags: ignoreversion
Source: help/images/dlg_config.png;            DestDir: {app}/help/images/; Flags: ignoreversion
Source: help/images/dlg_fonts.png;             DestDir: {app}/help/images/; Flags: ignoreversion
Source: help/images/radioSelectedBullet.png;   DestDir: {app}/help/images/; Flags: ignoreversion
Source: help/images/stampBullet.png;           DestDir: {app}/help/images/; Flags: ignoreversion


Source: examples/AlbumSample.txt;            DestDir: {userdocs}/AlbumEasy/examples/;    Flags: ignoreversion
Source: examples/Font.txt;                   DestDir: {userdocs}/AlbumEasy/examples/;    Flags: ignoreversion
Source: examples/International.txt;          DestDir: {userdocs}/AlbumEasy/examples/;    Flags: ignoreversion

Source: {#QtLocation}/bin/Qt5Core.dll;                 DestDir: {app};              Flags: ignoreversion
Source: {#QtLocation}/bin/Qt5Gui.dll;                  DestDir: {app};              Flags: ignoreversion
Source: {#QtLocation}/bin/Qt5Widgets.dll;              DestDir: {app};              Flags: ignoreversion
Source: {#QtLocation}/bin/icudt51.dll;                 DestDir: {app};              Flags: ignoreversion
Source: {#QtLocation}/bin/icuin51.dll;                 DestDir: {app};              Flags: ignoreversion
Source: {#QtLocation}/bin/icuuc51.dll;                 DestDir: {app};              Flags: ignoreversion
Source: {#QtLocation}/bin/libgcc_s_dw2-1.dll;          DestDir: {app};              Flags: ignoreversion
Source: {#QtLocation}/bin/libstdc++-6.dll;             DestDir: {app};              Flags: ignoreversion
Source: {#QtLocation}/bin/libwinpthread-1.dll;         DestDir: {app};              Flags: ignoreversion
Source: {#QtLocation}/plugins/platforms/qminimal.dll;  DestDir: {app}/platforms/;   Flags: ignoreversion
Source: {#QtLocation}/plugins/platforms/qwindows.dll;  DestDir: {app}/platforms/;   Flags: ignoreversion

[Icons]
Name: {userdesktop}\{#AppVersionName};                 Filename: {app}\AlbumEasy.exe; Tasks:desktopicon
Name: {group}\{#AppVersionName};                       Filename: {app}\AlbumEasy.exe
Name: {group}\{cm:UninstallProgram,{#AppVersionName}}; Filename: {uninstallexe}



