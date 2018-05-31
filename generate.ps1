cmake -E remove_directory build/x86-uwp-Debug
cmake -E make_directory build/x86-uwp-Debug
cmake -E chdir build/x86-uwp-Debug cmake -G "Visual Studio 15 2017" -DCMAKE_SYSTEM_NAME="WindowsStore"  -DCMAKE_SYSTEM_VERSION="10.0" ../..
