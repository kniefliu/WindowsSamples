# skia tools

- Auto compile tools
	- [python2.7](python-2.7.15.amd64.msi)
	- [gn](gn.exe)
	- [ninja](ninja.exe)
	- VS 2015, Microsoft Visual C++ 2015
	- [Windows 10 SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-10-sdk/) (Version 10.0.18362.0 or later)
		- ANGLE need Windows 10 SDK

- Use ninja compile
	- Set env path, run [SetToolPath.bat](SetToolPath.bat)
	- Generate vs2015 project: `bin\gn.exe gen out\ninja-Static --ide="vs2015" --sln="SampleApp" --args="is_official_build=false skia_use_angle=true angle_use_commit=false skia_use_egl=false skia_enable_spirv_validation=false target_cpu=\"x64\""`
	- Start vs2015, two methods: 
		- The first method: `start out\ninja-Static\SampleApp.sln`
		- The second method: `"C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\devenv.exe" out\ninja-Static\SampleApp.sln`
	- Compile and run SampleApp project

- Use ninja compile: skia dll
	- Set env path, run [SetToolPath.bat](SetToolPath.bat)
	- Generate vs2015 project: `bin\gn.exe gen out\ninja-Shared --ide="vs2015" --sln="skia" --args="is_component_build=true is_official_build=false skia_use_angle=true angle_use_commit=false skia_use_egl=false skia_enable_spirv_validation=false target_cpu=\"x64\""`
	- Start vs2015, two methods: 
		- The first method: `start out\ninja-Shared\skia.sln`
		- The second method: `"C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\devenv.exe" out\ninja-Shared\skia.sln`
	- Compile and run SampleApp project


- Use ninja compile: skia dll
	- Set env path, run [SetToolPath.bat](SetToolPath.bat)
	- Generate vs2015 project: `bin\gn.exe gen out-x64\ninja-Shared --ide="vs2015" --sln="skia" --args="is_component_build=true is_official_build=true skia_use_angle=false skia_use_egl=false skia_enable_spirv_validation=false skia_use_expat=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_zlib=false skia_enable_pdf=false target_cpu=\"x64\""`
	- Start vs2015, two methods: 
		- The first method: `start out-x64\ninja-Shared\skia.sln`
		- The second method: `"C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\devenv.exe" out-x64\ninja-Shared\skia.sln`
	- Compile and run SampleApp project
