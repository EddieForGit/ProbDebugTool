:: ############################################################################
:: 方案開啟前置作業
:: ############################################################################
@ECHO OFF

SETLOCAL ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION

REM 先清空畫面
CLS

REM 設定要開啟的方案
SET sln_file=DebugSettingTool.sln

:: ############################################################################
:: 指令設定
:: ############################################################################
SET CMD_START= START
REM SET CMD_START= START /WAIT  /B /D
SET CMD_MoveFolder= CHDIR /D

ECHO =============== [ Cmd Setting ] ==================
:: ############################################################################
:: 開發環境設定
:: ############################################################################
SET current_floder=%CD%
ECHO Current floder in [%current_floder%]

%CMD_MoveFolder% %~dp0%

ECHO Move to %CD%

REM 設定方案路徑
SET sln_path=%CD%

:: ############################################################################
:: Qt開發環境設定
:: ############################################################################
SET QtVersion=5.5.1
SET QT="QtDir=C:\Qt\%QtVersion%"
SET QtDir=C:\Qt\%QtVersion%
SET PATH=%QtDir%\bin;%PATH%

ECHO.
ECHO =============== [ QT Setting ] ===================
ECHO Version: [%QtVersion%]
ECHO PATH: [%QtDir%]

:: ############################################################################
:: IDE使用設定
:: ############################################################################
REM 版號
SET vs_version=14
REM IDE 路徑
SET vs_path="C:\Program Files (x86)\Microsoft Visual Studio %vs_version%.0\Common7\IDE"
REM 設定IDE類型
SET vs_type=Express
REM IDE執行檔名稱
SET vs_ide_name=
IF %vs_type% == Express (
	SET vs_ide_name=WDExpress.exe
) ELSE (
	SET vs_ide_name=devenv.exe
)

ECHO.
ECHO =============== [ IDE Setting ] ===================
ECHO Path: %vs_path%\%vs_ide_name%
ECHO Version: %vs_version%

ECHO.
ECHO =============== Running =======================
ECHO %vs_ide_name% %sln_path%\%sln_file%

:: ############################################################################
:: 開啟方案
:: ############################################################################
REM 啟動參數設定
SET cmd_start_param_vs= /D !vs_path! /WAIT /B
%CMD_START% %cmd_start_param_vs%  %vs_ide_name% %sln_path%\%sln_file%

:EOF
REM 切回原本目錄
%CMD_MoveFolder% %current_floder%
ECHO Close sln file and move back %current_floder%

@ECHO ON
