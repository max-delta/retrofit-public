@REM VS2017 Batch Build routinely corrupts all your build artifacts. Known issue...
@REM Work-around is to roll your own batch file :/

@cls
@cd /D "%~dp0"

@echo Low verbosity! May sit for a while without messages!
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x64 /p:Configuration=Debug
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x64 /p:Configuration=Release
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x64 /p:Configuration=Retail
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86 /p:Configuration=Debug
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86 /p:Configuration=Release
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86 /p:Configuration=Retail
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=LLVM_x64 /p:Configuration=Debug
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=LLVM_x64 /p:Configuration=Release
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=LLVM_x64 /p:Configuration=Retail
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM /p:Configuration=Debug
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM /p:Configuration=Release
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM /p:Configuration=Retail
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM64 /p:Configuration=Debug
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM64 /p:Configuration=Release
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM64 /p:Configuration=Retail
