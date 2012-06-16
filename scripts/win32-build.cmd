@echo off
setlocal EnableDelayedExpansion
setlocal enableextensions 

REM no options / set to default
set BuildType=Release
set InitEnvScript=win32-init_env_command.cmd
if /i "%1"=="-h" (
	echo Usage: win32-build.cmd [Build Type] [Init-env Script]
	echo -- Build Type option can be : --release (-r^) or --debug (-d^). Default is release.
	echo -- Default Init-env script is: win32-init_env_command.cmd
	pause
	exit 0
) else if /i "%1"=="--help" (
	echo Usage: win32-build.cmd [Build Type] [Init-env Script]
	echo -- Build Type option can be : --release (-r^) or --debug (-d^). Default is Release.
	echo -- Default Init-env script is: win32-init_env_command.cmd
	pause
	exit 0
) else if /i "%1"=="-d" (
	set BuildType=Debug
	set InitEnvScript=%2
) else if /i "%1"=="--debug" (
	set BuildType=Debug
	set InitEnvScript=%2
) else if /i "%1"=="-r" (
	set BuildType=Release
	set InitEnvScript=%2
) else if /i "%1"=="--release" (
	set BuildType=Release
	set InitEnvScript=%2
) else if not "%1"=="" (
	set BuildType=Release
	set InitEnvScript=%1
)

if /i "%InitEnvScript%"=="" (
	echo No script specified. Default will be used.
	set InitEnvScript=win32-init_env_command.cmd
)

echo --
echo build type is set to: %BuildType%.
echo Init-env Script to be called: %InitEnvScript%.
echo --


REM #######################################################################################

if not exist "%InitEnvScript%" (
	if not "%InitEnvScript%"=="win32-init_env_command.cmd" (
		echo `%InitEnvScript%` file not found !
		echo Init-env script used : "win32-init_env_command.cmd"
		set InitEnvScript="win32-init_env_command.cmd"
	)
	if not exist "win32-init_env_command.cmd" (
		echo "win32-init_env_command.cmd" not found !
		echo `win32-init_env_command.cmd-skeleton` is a good file to start with... (I'll copy this file for you^)
		copy "win32-init_env_command.cmd-skeleton" "win32-init_env_command.cmd"
	)
)

call "%InitEnvScript%"

REM #######################################################################################

set saved_directory=%CD%
set target_dist=..\dist

echo.
echo _______________________________________________________________________________
echo.

mkdir ..\local-tmp 2> NULL
for /F %%s in (%OpenViBE_build_order%) do (
	set OpenViBE_project_name_full=%%s
	set OpenViBE_project_name_rel=!OpenViBE_project_name_full:%OpenViBE_base%\=!
	echo Configuring and building !OpenViBE_project_name_rel! ...
	echo.

	mkdir ..\local-tmp\!OpenViBE_project_name_rel! 2> NULL
	cd ..\local-tmp\!OpenViBE_project_name_rel!

	cmake -DCMAKE_INSTALL_PREFIX="%%s" -DCMAKE_BUILD_TYPE=%BuildType% -DCMAKE_CXX_FLAGS=" /DWIN32 /D_WINDOWS /W3 /Zm1000 /EHsc /GR /wd4355" -Wno-dev -DCMAKE_MODULE_PATH="%saved_directory:\=/%/../cmake-modules;${CMAKE_MODULE_PATH}" !OpenViBE_project_name_full! -G"NMake Makefiles"
	IF NOT "!ERRORLEVEL!" == "0" goto terminate_error

	nmake
	IF NOT "!ERRORLEVEL!" == "0" goto terminate_error

	nmake OpenViBE-documentation 2> NULL
	REM IF NOT "!ERRORLEVEL!" == "0" goto terminate_error

	nmake install
	REM IF NOT "!ERRORLEVEL!" == "0" goto terminate_error

	cd %saved_directory%

	echo.
	echo _______________________________________________________________________________
	echo.
)

echo.
echo Building process terminated successfully !
echo.

REM #######################################################################################

echo.
echo Installing files to 'dist' folder, this can take a few seconds - maybe a few minutes...
echo.

REM rmdir /s /q %target_dist%         > NULL 2<&1

mkdir %target_dist%               > NULL 2<&1
mkdir %target_dist%\bin           > NULL 2<&1
mkdir %target_dist%\include       > NULL 2<&1
mkdir %target_dist%\lib           > NULL 2<&1
mkdir %target_dist%\share         > NULL 2<&1
mkdir %target_dist%\etc           > NULL 2<&1
mkdir %target_dist%\doc           > NULL 2<&1
mkdir %target_dist%\log           > NULL 2<&1
mkdir %target_dist%\tmp           > NULL 2<&1

echo @echo off                                                            >  %target_dist%\ov-vr-demo-tie-fighter.cmd
echo SET OpenViBE_DistRoot=%%CD%%>> %target_dist%\ov-vr-demo-tie-fighter.cmd
echo pushd ..\scripts                                                     >> %target_dist%\ov-vr-demo-tie-fighter.cmd
echo call %InitEnvScript%                                                 >> %target_dist%\ov-vr-demo-tie-fighter.cmd
echo popd                                                                 >> %target_dist%\ov-vr-demo-tie-fighter.cmd
echo copy share\openvibe-applications\vr-demo\tie-fighter\resources.cfg-base share\openvibe-applications\vr-demo\tie-fighter\resources.cfg >> %target_dist%\ov-vr-demo-tie-fighter.cmd
echo type %OV_DEP_CEGUI%\resources.cfg ^>^> share\openvibe-applications\vr-demo\tie-fighter\resources.cfg >> %target_dist%\ov-vr-demo-tie-fighter.cmd
echo cd share\openvibe-applications\vr-demo\tie-fighter                   >> %target_dist%\ov-vr-demo-tie-fighter.cmd
echo %%OpenViBE_DistRoot%%\bin\OpenViBE-vr-demo-dynamic.exe tie-fighter   >> %target_dist%\ov-vr-demo-tie-fighter.cmd
echo cd %%OpenViBE_DistRoot%%                                             >> %target_dist%\ov-vr-demo-tie-fighter.cmd
echo exit                                                                 >> %target_dist%\ov-vr-demo-tie-fighter.cmd

echo @echo off                                                            >  %target_dist%\ov-vr-demo-handball.cmd
echo SET OpenViBE_DistRoot=%%CD%%>> %target_dist%\ov-vr-demo-handball.cmd
echo pushd ..\scripts                                                     >> %target_dist%\ov-vr-demo-handball.cmd
echo call %InitEnvScript%                                                 >> %target_dist%\ov-vr-demo-handball.cmd
echo popd                                                                 >> %target_dist%\ov-vr-demo-handball.cmd
echo copy share\openvibe-applications\vr-demo\handball\resources.cfg-base share\openvibe-applications\vr-demo\handball\resources.cfg >> %target_dist%\ov-vr-demo-handball.cmd
echo type %OV_DEP_CEGUI%\resources.cfg ^>^> share\openvibe-applications\vr-demo\handball\resources.cfg >> %target_dist%\ov-vr-demo-handball.cmd
echo cd share\openvibe-applications\vr-demo\handball                      >> %target_dist%\ov-vr-demo-handball.cmd
echo %%OpenViBE_DistRoot%%\bin\OpenViBE-vr-demo-dynamic.exe handball      >> %target_dist%\ov-vr-demo-handball.cmd
echo cd %%OpenViBE_DistRoot%%                                             >> %target_dist%\ov-vr-demo-handball.cmd
echo exit                                                                 >> %target_dist%\ov-vr-demo-handball.cmd

echo @echo off                                               >  %target_dist%\ov-acquisition-server.cmd
echo pushd ..\scripts                                        >> %target_dist%\ov-acquisition-server.cmd
echo call %InitEnvScript%                                    >> %target_dist%\ov-acquisition-server.cmd
echo popd                                                    >> %target_dist%\ov-acquisition-server.cmd
echo cd bin                                                  >> %target_dist%\ov-acquisition-server.cmd
echo OpenViBE-acquisition-server-dynamic.exe %%1 %%2 %%3 %%4 >> %target_dist%\ov-acquisition-server.cmd
echo pause                                                   >> %target_dist%\ov-acquisition-server.cmd

echo @echo off                                               >  %target_dist%\ov-designer.cmd
echo pushd ..\scripts                                        >> %target_dist%\ov-designer.cmd
echo call %InitEnvScript%                                    >> %target_dist%\ov-designer.cmd
echo popd                                                    >> %target_dist%\ov-designer.cmd
echo cd bin                                                  >> %target_dist%\ov-designer.cmd
echo OpenViBE-designer-dynamic.exe %%1 %%2 %%3 %%4 %%5 %%6   >> %target_dist%\ov-designer.cmd
echo pause                                                   >> %target_dist%\ov-designer.cmd

echo @echo off                                               >  %target_dist%\ov-id-generator.cmd
echo pushd ..\scripts                                        >> %target_dist%\ov-id-generator.cmd
echo call %InitEnvScript%                                    >> %target_dist%\ov-id-generator.cmd
echo popd                                                    >> %target_dist%\ov-id-generator.cmd
echo cd bin                                                  >> %target_dist%\ov-id-generator.cmd
echo OpenViBE-id-generator-dynamic.exe %%1 %%2 %%3 %%4 %%5   >> %target_dist%\ov-id-generator.cmd
echo pause                                                   >> %target_dist%\ov-id-generator.cmd

echo @echo off                                               >  %target_dist%\ov-plugin-inspector.cmd
echo pushd ..\scripts                                        >> %target_dist%\ov-plugin-inspector.cmd
echo call %InitEnvScript%                                    >> %target_dist%\ov-plugin-inspector.cmd
echo popd                                                    >> %target_dist%\ov-plugin-inspector.cmd
echo cd bin                                                  >> %target_dist%\ov-plugin-inspector.cmd
echo OpenViBE-plugin-inspector-dynamic.exe %%1 %%2 %%3 %%4   >> %target_dist%\ov-plugin-inspector.cmd
echo pause                                                   >> %target_dist%\ov-plugin-inspector.cmd

echo @echo off                                               >  %target_dist%\ov-skeleton-generator.cmd
echo pushd ..\scripts                                        >> %target_dist%\ov-skeleton-generator.cmd
echo call win32-init_env_command.cmd                         >> %target_dist%\ov-skeleton-generator.cmd
echo popd                                                    >> %target_dist%\ov-skeleton-generator.cmd
echo cd bin                                                  >> %target_dist%\ov-skeleton-generator.cmd
echo OpenViBE-skeleton-generator-dynamic.exe %%1 %%2 %%3 %%4 >> %target_dist%\ov-skeleton-generator.cmd
echo pause                                                   >> %target_dist%\ov-skeleton-generator.cmd

echo @echo off                                               >  %target_dist%\ov-ssvep-demo-training.cmd
echo pushd ..\scripts                                        >> %target_dist%\ov-ssvep-demo-training.cmd
echo call win32-init_env_command.cmd                         >> %target_dist%\ov-ssvep-demo-training.cmd
echo popd                                                    >> %target_dist%\ov-ssvep-demo-training.cmd
echo cd bin                                                  >> %target_dist%\ov-ssvep-demo-training.cmd
echo start OpenViBE-ssvep-demo-dynamic.exe training	         >> %target_dist%\ov-ssvep-demo-training.cmd

echo @echo off                                               >  %target_dist%\ov-ssvep-demo-shooter.cmd
echo pushd ..\scripts                                        >> %target_dist%\ov-ssvep-demo-shooter.cmd
echo call win32-init_env_command.cmd                         >> %target_dist%\ov-ssvep-demo-shooter.cmd
echo popd                                                    >> %target_dist%\ov-ssvep-demo-shooter.cmd
echo cd bin                                                  >> %target_dist%\ov-ssvep-demo-shooter.cmd
echo start OpenViBE-ssvep-demo-dynamic.exe shooter           >> %target_dist%\ov-ssvep-demo-shooter.cmd

echo @echo off                                               >  %target_dist%\bin\OpenViBE-external-application-launcher.cmd
echo pushd ..                                                >> %target_dist%\bin\OpenViBE-external-application-launcher.cmd
echo ov-%%1.cmd %%2 %%3 %%4 %%5                              >> %target_dist%\bin\OpenViBE-external-application-launcher.cmd


echo.
for /F %%s in (%OpenViBE_build_order%) do (
	set OpenViBE_project_name_full=%%s
	set OpenViBE_project_name_rel=!OpenViBE_project_name_full:%OpenViBE_base%\=!
	echo ## Installing !OpenViBE_project_name_rel! ...
	xcopy /q /e /y /c %%s\bin\*.*         %target_dist%\bin     >> NULL
	xcopy /q /e /y %%s\include\*.*        %target_dist%\include >> NULL
	xcopy /q /e /y %%s\lib\*.*            %target_dist%\lib     >> NULL
	xcopy /q /e /y %%s\share\*.*          %target_dist%\share   >> NULL
	xcopy /q /e /y %%s\etc\*.*            %target_dist%\etc     >> NULL
	xcopy /q /e /y %%s\doc\*.*            %target_dist%\doc     >> NULL
)
echo.

echo.
echo Install completed !
echo.

goto terminate_success

REM #######################################################################################

:terminate_error

echo.
echo An error occured during building process !
echo.
pause

goto terminate

REM #######################################################################################

:terminate_success

pause

goto terminate

REM #######################################################################################

:terminate

if exist NULL del NULL
