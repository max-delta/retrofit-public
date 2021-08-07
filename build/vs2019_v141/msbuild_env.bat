REM
REM VS2019 command prompt's MSBuild doesn't properly respect v141 settings
REM

SET PATH=%PATH%;"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\"
cd ..
cd ..
%comspec%
