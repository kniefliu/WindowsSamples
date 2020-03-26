@ECHO OFF

SET SKIA_M63_BIN_PATH=%~dp0
SET PATH=%SKIA_M63_BIN_PATH%;C:\Python27;%PATH%

rem %comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"" x64

CD ../

CMD /K