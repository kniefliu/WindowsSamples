# skia vs2019

- **msvc\_project\_tool.py** is a tool that replace ninja.exe in vs2019 project to cl.exe
	- Examples: 
		- `python msvc_project_tool.py -src="%SKIA_DIR%/skia_m76/vs2019/ninja-Static" -dest="%SKIA_DIR%/skia_m76/vs2019/Static"`
