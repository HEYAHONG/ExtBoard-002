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


@rem 清理工程目录
echo APP:%APP_ROOT_PATH%
pushd %APP_ROOT_PATH:/=\%\..\sdk
xmake clean --all -v
popd