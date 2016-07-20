SET QtVersion=5.5.1
SET QT="QtDir=C:\Qt\%QtVersion%"
SET QtDir=C:\Qt\%QtVersion%
SET PATH=%QtDir%\bin;%PATH%
START ProbDebugTool.sln
pause