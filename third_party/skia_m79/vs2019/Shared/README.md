# skia vs2019

- skia dll
	- Compile & Run
		- **ninja to vs2019**: src(ninja full path); dest(vs2019 full path); `python msvc_project_tool.py -src="./ninja-Shared" -dest="./Shared"`
		- **libGLESv2** project need link these libs: **d3d9.lib;dxgi.lib**
		- **libEGL** project need link these libs: **libGLESv2.lib**
		- **skia** link command pack all libs.
			- link add command: `/WHOLEARCHIVE:gpu /WHOLEARCHIVE:pdf /WHOLEARCHIVE:skcms /WHOLEARCHIVE:avx /WHOLEARCHIVE:fontmgr_win /WHOLEARCHIVE:fontmgr_win_gdi /WHOLEARCHIVE:gif /WHOLEARCHIVE:heif /WHOLEARCHIVE:hsw /WHOLEARCHIVE:jpeg /WHOLEARCHIVE:png /WHOLEARCHIVE:sse41 /WHOLEARCHIVE:sse42 /WHOLEARCHIVE:ssse3 /WHOLEARCHIVE:webp /WHOLEARCHIVE:xml `
		- **HelloWorld** project
			- link libs: `skia.lib libEGL.lib OpenGL32.lib`
			- Run:
		- All projects
			- 优化 | 已禁用 (/Od)