@echo off
setlocal EnableDelayedExpansion
setlocal enableextensions 
REM #######################################################################################

if not exist "win32-init_env_command.cmd" (
	echo You have to configure your `win32-init_env_command` file.
	echo `win32-init_env_command-skeleton` is a good file to start with...
	goto terminate
)

call "win32-init_env_command.cmd"

echo.
echo.
echo IMPORTANT NOTICE:
echo.
echo This script can generate VCProj files in order to help you in the task of debugging some
echo parts of the code - visual knows what files to load and the projects structure -
echo This script is _not_ intended to be used in order to actually _build_ the platform
echo which is 'win32-build.cmd's task. Additionnaly, note that the platform has to be
echo compiled entirely at least once with 'win32-build.cmd' before thinking of generating
echo VCProj files.
echo.
echo VCProj files and solutions will go in local-tmp/visual folder.
echo.
echo If you don't understand this notice, you should consider not using this script and
echo press CTRL+C to abort. Thank you.
echo.
echo.
pause

REM #######################################################################################

set OB=(
set CB=!OB:(=)!

set saved_directory=%CD%
set target_dist=..\dist

rmdir /s /q ..\local-tmp\visual > NULL 2<&1
mkdir ..\local-tmp\visual 2> NULL
del   ..\local-tmp\visual\CMakeLists.txt 2> NULL
for /F %%s in (%OpenViBE_build_order%) do (
	set OpenViBE_project_name_full=%%s
	set OpenViBE_project_name_rel=!OpenViBE_project_name_full:%OpenViBE_base%\=!

	echo SUBDIRS!OB!"!OpenViBE_project_name_full:\=/!"!CB! >> ..\local-tmp\visual\CMakeLists.txt
)

cd ..\local-tmp\visual
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=" /DWIN32 /D_WINDOWS /W3 /Zm1000 /EHsc /GR /wd4355" -Wno-dev -DCMAKE_MODULE_PATH="%saved_directory:\=/%/../cmake-modules;${CMAKE_MODULE_PATH}" . -G"Visual Studio 10"
cd %saved_directory%

echo.
echo Visual studio projects generation terminated successfully !
echo.

pause

:terminate

del NULL
del CMakeLists.txt
