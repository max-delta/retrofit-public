@REM VS2017 Batch Build routinely corrupts all your build artifacts. Known issue...
@REM Work-around is to roll your own batch file :/

@cls
@cd /D "%~dp0"

@echo Low verbosity! May sit for a while without messages!

msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM32 /p:Configuration=LLVM_Debug
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM32 /p:Configuration=LLVM_Release
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM32 /p:Configuration=LLVM_Retail
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM64 /p:Configuration=LLVM_Debug
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM64 /p:Configuration=LLVM_Release
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM64 /p:Configuration=LLVM_Retail

@REM VS2022 Clang-cl is fucked somehow now, where it rebuilds every time:
@REM   All outputs are up-to-date.
@REM   All source files are not up-to-date:  command line has changed since the last build.
@REM  But there doesn't seem to be any way to tell what the fuck it thinks changed?
@REM  It even has some output later that shows to commands that are identical:
@REM   Source "OverloadNew.cpp" doesn't match previous command line.
@REM   *** Previous command line: '...
@REM   *** Current  command line: '...
@REM Workaround is to invoke full-on VS, since apparently that no longer invokes
@REM  MSBuild directly, and instead rolls its own weird shit for processing
@REM msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_32 /p:Configuration=LLVM_Debug
@REM msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_32 /p:Configuration=LLVM_Release
@REM msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_32 /p:Configuration=LLVM_Retail
@REM msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_64 /p:Configuration=LLVM_Debug
@REM msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_64 /p:Configuration=LLVM_Release
@REM msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_64 /p:Configuration=LLVM_Retail
devenv retrofit.sln /Build "LLVM_Debug|x86_32"
devenv retrofit.sln /Build "LLVM_Release|x86_32"
devenv retrofit.sln /Build "LLVM_Retail|x86_32"
devenv retrofit.sln /Build "LLVM_Debug|x86_64"
devenv retrofit.sln /Build "LLVM_Release|x86_64"
devenv retrofit.sln /Build "LLVM_Retail|x86_64"

msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM32 /p:Configuration=MSVC_Debug
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM32 /p:Configuration=MSVC_Release
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM32 /p:Configuration=MSVC_Retail
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM64 /p:Configuration=MSVC_Debug
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM64 /p:Configuration=MSVC_Release
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=ARM64 /p:Configuration=MSVC_Retail
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_32 /p:Configuration=MSVC_Debug
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_32 /p:Configuration=MSVC_Release
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_32 /p:Configuration=MSVC_Retail
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_64 /p:Configuration=MSVC_Debug
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_64 /p:Configuration=MSVC_Release
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_64 /p:Configuration=MSVC_Retail
