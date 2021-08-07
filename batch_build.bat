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
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_32 /p:Configuration=LLVM_Debug
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_32 /p:Configuration=LLVM_Release
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_32 /p:Configuration=LLVM_Retail
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_64 /p:Configuration=LLVM_Debug
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_64 /p:Configuration=LLVM_Release
msbuild.exe retrofit.sln /nologo /verbosity:q /maxcpucount /target:Build /p:Platform=x86_64 /p:Configuration=LLVM_Retail

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
