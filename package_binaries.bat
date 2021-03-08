@cls
@cd /D "%~dp0"

mkdir _binaries
mkdir _binaries\test

copy _artifacts\x86_64\MSVC_Retail\Out\core_test.exe _binaries\test\core_test_x64.exe
copy _artifacts\x86_64\MSVC_Retail\Out\core_test.pdb _binaries\test\core_test_x64.pdb
copy _artifacts\x86_64\MSVC_Retail\Out\foundation_test.exe _binaries\test\foundation_test_x64.exe
copy _artifacts\x86_64\MSVC_Retail\Out\foundation_test.pdb _binaries\test\foundation_test_x64.pdb
copy _artifacts\x86_32\MSVC_Retail\Out\core_test.exe _binaries\test\core_test_x86.exe
copy _artifacts\x86_32\MSVC_Retail\Out\core_test.pdb _binaries\test\core_test_x86.pdb
copy _artifacts\x86_32\MSVC_Retail\Out\foundation_test.exe _binaries\test\foundation_test_x86.exe
copy _artifacts\x86_32\MSVC_Retail\Out\foundation_test.pdb _binaries\test\foundation_test_x86.pdb
copy _artifacts\x86_64\LLVM_Retail\Out\core_test.exe _binaries\test\core_test_LLVM.exe
copy _artifacts\x86_64\LLVM_Retail\Out\core_test.pdb _binaries\test\core_test_LLVM.pdb
copy _artifacts\x86_64\LLVM_Retail\Out\foundation_test.exe _binaries\test\foundation_test_LLVM.exe
copy _artifacts\x86_64\LLVM_Retail\Out\foundation_test.pdb _binaries\test\foundation_test_LLVM.pdb
copy _artifacts\x86_32\LLVM_Retail\Out\core_test.exe _binaries\test\core_test_LLVM32.exe
copy _artifacts\x86_32\LLVM_Retail\Out\core_test.pdb _binaries\test\core_test_LLVM32.pdb
copy _artifacts\x86_32\LLVM_Retail\Out\foundation_test.exe _binaries\test\foundation_test_LLVM32.exe
copy _artifacts\x86_32\LLVM_Retail\Out\foundation_test.pdb _binaries\test\foundation_test_LLVM32.pdb

copy _artifacts\x86_64\MSVC_Retail\Out\retrofit.dll _binaries\retrofit.dll
copy _artifacts\x86_64\MSVC_Retail\Out\retrofit.pdb _binaries\retrofit.pdb
copy _artifacts\x86_64\MSVC_Retail\Out\retrofit_exe.exe _binaries\retrofit.exe
copy _artifacts\x86_64\MSVC_Retail\Out\retrofit_exe.pdb _binaries\retrofit_exe.pdb
copy _artifacts\x86_64\MSVC_Retail\Out\FramePackEditor.exe _binaries\FramePackEditor.exe
copy _artifacts\x86_64\MSVC_Retail\Out\FramePackEditor.pdb _binaries\FramePackEditor.pdb
copy _artifacts\x86_64\MSVC_Retail\Out\cc3o3.dll _binaries\cc3o3.dll
copy _artifacts\x86_64\MSVC_Retail\Out\cc3o3.pdb _binaries\cc3o3.pdb
copy _artifacts\x86_64\MSVC_Retail\Out\cc3o3_exe.exe _binaries\cc3o3.exe
copy _artifacts\x86_64\MSVC_Retail\Out\cc3o3_exe.pdb _binaries\cc3o3_exe.pdb
