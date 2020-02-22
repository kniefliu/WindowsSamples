# skia vs2015

- **msvc\_project\_tool.py** is a tool that replace ninja.exe in vs2015 project to cl.exe
	- Examples: 
		- `python msvc_project_tool.py -src="%SKIA_DIR%/skia_m63/vs2015/ninja-Static" -dest="%SKIA_DIR%/skia_m63/vs2015/Static"`
