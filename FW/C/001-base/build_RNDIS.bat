@echo off

@rem 目录
set APP_ROOT_PATH=%~dp0

@rem 设置sdk所需环境变量设置(可根据实际情况设置环境变量)
set EC618_RNDIS=enable

@rem 构建固件
call "%APP_ROOT_PATH%\build.bat"

