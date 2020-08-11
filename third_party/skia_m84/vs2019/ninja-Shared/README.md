# skia vs2019

- skia dll
	- Compile & Run
		- **ninja to vs2019**: src(ninja full path); dest(vs2019 full path); `python msvc_project_tool.py -src="./ninja-Shared" -dest="./Shared"`
		- **libGLESv2** project need link these libs: **d3d9.lib;dxgi.lib**
		- **libEGL** project need link these libs: **libGLESv2.lib**
		- **skia** project need link these libs: **expat.lib;libjpeg.lib;libpng.lib;libwebp.lib;libwebp_sse41.lib;zlib.lib;zlib_x86.lib;FontSub.lib;Usp10.lib;OpenGL32.lib**
		- **skia** link command pack all libs.
			- link add command: `/WHOLEARCHIVE:avx /WHOLEARCHIVE:experimental_svg_model /WHOLEARCHIVE:fontmgr_win /WHOLEARCHIVE:fontmgr_win_gdi  /WHOLEARCHIVE:gif /WHOLEARCHIVE:gpu /WHOLEARCHIVE:heif  /WHOLEARCHIVE:hsw /WHOLEARCHIVE:jpeg_decode /WHOLEARCHIVE:jpeg_encode /WHOLEARCHIVE:module_skottie /WHOLEARCHIVE:pdf /WHOLEARCHIVE:png_decode /WHOLEARCHIVE:png_encode /WHOLEARCHIVE:skcms /WHOLEARCHIVE:sse41 /WHOLEARCHIVE:sse42 /WHOLEARCHIVE:ssse3 /WHOLEARCHIVE:webp_decode /WHOLEARCHIVE:webp_encode /WHOLEARCHIVE:xml /WHOLEARCHIVE:xps `
		- **HelloWorld** project
			- link libs: `sk_app.lib;skia.lib;libEGL.lib;OpenGL32.lib`
			- Run:
		- All projects
			- 优化 | 已禁用 (/Od)