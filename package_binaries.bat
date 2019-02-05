@cls
@cd /D "%~dp0"

mkdir _binaries

copy _artifacts\x64\Retail\Out\core_test.exe _binaries\core_test_x64.exe
copy _artifacts\Win32\Retail\Out\core_test.exe _binaries\core_test_x86.exe
copy _artifacts\LLVM_x64\Retail\Out\core_test.exe _binaries\core_test_LLVM.exe

copy _artifacts\x64\Retail\Out\core_test.pdb _binaries\core_test_x64.pdb
copy _artifacts\Win32\Retail\Out\core_test.pdb _binaries\core_test_x86.pdb
copy _artifacts\LLVM_x64\Retail\Out\core_test.pdb _binaries\core_test_LLVM.pdb

copy _artifacts\x64\Retail\Out\foundation_test.exe _binaries\foundation_test_x64.exe
copy _artifacts\Win32\Retail\Out\foundation_test.exe _binaries\foundation_test_x86.exe
copy _artifacts\LLVM_x64\Retail\Out\foundation_test.exe _binaries\foundation_test_LLVM.exe

copy _artifacts\x64\Retail\Out\foundation_test.pdb _binaries\foundation_test_x64.pdb
copy _artifacts\Win32\Retail\Out\foundation_test.pdb _binaries\foundation_test_x86.pdb
copy _artifacts\LLVM_x64\Retail\Out\foundation_test.pdb _binaries\foundation_test_LLVM.pdb

copy _artifacts\x64\Retail\Out\retrofit.exe _binaries\retrofit_x64.exe
copy _artifacts\Win32\Retail\Out\retrofit.exe _binaries\retrofit_x86.exe
copy _artifacts\LLVM_x64\Retail\Out\retrofit.exe _binaries\retrofit_LLVM.exe

copy _artifacts\x64\Retail\Out\retrofit.pdb _binaries\retrofit_x64.pdb
copy _artifacts\Win32\Retail\Out\retrofit.pdb _binaries\retrofit_x86.pdb
copy _artifacts\LLVM_x64\Retail\Out\retrofit.pdb _binaries\retrofit_LLVM.pdb

copy _artifacts\x64\Retail\Out\FramePackEditor.exe _binaries\FramePackEditor_x64.exe
copy _artifacts\Win32\Retail\Out\FramePackEditor.exe _binaries\FramePackEditor_x86.exe
copy _artifacts\LLVM_x64\Retail\Out\FramePackEditor.exe _binaries\FramePackEditor_LLVM.exe

copy _artifacts\x64\Retail\Out\FramePackEditor.pdb _binaries\FramePackEditor_x64.pdb
copy _artifacts\Win32\Retail\Out\FramePackEditor.pdb _binaries\FramePackEditor_x86.pdb
copy _artifacts\LLVM_x64\Retail\Out\FramePackEditor.pdb _binaries\FramePackEditor_LLVM.pdb

copy _artifacts\x64\Retail\Out\cc3o3.exe _binaries\cc3o3_x64.exe
copy _artifacts\Win32\Retail\Out\cc3o3.exe _binaries\cc3o3_x86.exe
copy _artifacts\LLVM_x64\Retail\Out\cc3o3.exe _binaries\cc3o3_LLVM.exe

copy _artifacts\x64\Retail\Out\cc3o3.pdb _binaries\cc3o3_x64.pdb
copy _artifacts\Win32\Retail\Out\cc3o3.pdb _binaries\cc3o3_x86.pdb
copy _artifacts\LLVM_x64\Retail\Out\cc3o3.pdb _binaries\cc3o3_LLVM.pdb
