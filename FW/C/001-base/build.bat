@echo off

@rem 目录
set APP_ROOT_PATH=%~dp0

@rem 设置sdk所需环境变量设置(可根据实际情况设置环境变量)
set PROJECT_NAME=ExtBoard-002-001-base
set PROJECT_DIR=%APP_ROOT_PATH%\
set PROJECT_DIR=%PROJECT_DIR:\=/%
if "X" == "X%LSPD_MODE%" set LSPD_MODE=disable
if "X" == "X%EC618_RNDIS%" set EC618_RNDIS=disable

@rem 检查SDK
if exist "%APP_ROOT_PATH%\..\sdk\xmake.lua" goto :CheckSdkOk
echo SDK不存在,请使用git下载并使用git submodule update --init下载子模块。
pause
goto :eof
:CheckSdkOk

@rem 检查xmake
xmake --version 2> nul  > nul
if "X%ERRORLEVEL%" == "X0" goto :CheckXMakeOk
echo xmake 未安装，请到https://xmake.io下载安装
pause
goto :eof
:CheckXMakeOk

@rem 构建工程并产生CMakeLists.txt(用于方便使用编辑器打开)
PUSHD "%APP_ROOT_PATH%\..\sdk" 2> nul > nul
xmake -y -w
if NOT "X%ERRORLEVEL%" == "X0" PAUSE
xmake  project -k cmake
POPD 2> nul > nul

@rem 使用ping延时一段时间
ping localhost 2> nul > nul


@rem 检查cmake
cmake --version 2> nul  > nul
if "X%ERRORLEVEL%" == "X0" goto :CheckCMakeOk
echo cmake 未安装，请到https://cmake.org下载安装
goto :eof
:CheckCMakeOk

@rem 检查ninja
ninja --version 2> nul  > nul
if "X%ERRORLEVEL%" == "X0" goto :CheckNinjaOk
echo Ninja 未安装，请到https://ninja-build.org/下载安装
goto :eof
:CheckNinjaOk

@rem 检查gcc
gcc --version 2> nul  > nul
if "X%ERRORLEVEL%" == "X0" goto :CheckGccOk
echo GCC未安装，无法生成工程。
goto :eof
:CheckGccOk


@rem 生成codeblocks工程文件
mkdir  "%APP_ROOT_PATH%\build"
pushd "%APP_ROOT_PATH%\build"
cmake -G "CodeBlocks - Ninja" "%APP_ROOT_PATH%\..\sdk"
popd



