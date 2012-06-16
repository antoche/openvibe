@echo off

if "%1" == "" goto recall_me

goto base

REM #######################################################################################

:recall_me

cmd /e /v /c %0 dummy

goto terminate

REM #######################################################################################

:base

REM #######################################################################################

if not exist "win32-init_env_command.cmd" (
	echo You have to configure your `win32-init_env_command` file.
	echo `win32-init_env_command-skeleton` is a good file to start with...
	goto terminate
)

call "win32-init_env_command.cmd"

REM #######################################################################################

set saved_directory=%CD%
set target_dist=..\dist

mkdir ..\local-tmp 2> NULL
for /F %%s in (%OpenViBE_build_order%) do (
	set OpenViBE_project_name_full=%%s
	set OpenViBE_project_name_rel=!OpenViBE_project_name_full:%OpenViBE_base%\=!

	mkdir ..\local-tmp\!OpenViBE_project_name_rel! 2> NULL
	cd ..\local-tmp\!OpenViBE_project_name_rel!
	cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=" /DWIN32 /D_WINDOWS /W3 /Zm1000 /EHsc /GR /wd4355" -Wno-dev -DCMAKE_MODULE_PATH="%saved_directory:\=/%/../cmake-modules;${CMAKE_MODULE_PATH}" !OpenViBE_project_name_full! -G"NMake Makefiles"
	nmake clean
	cd %saved_directory%
)
rmdir /s /q ..\local-tmp > NULL 2<&1

echo.
echo Clean process terminated successfully !
echo.

pause

:terminate

del NULL
