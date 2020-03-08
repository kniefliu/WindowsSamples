@ECHO OFF

SET SKIA_M63_BIN_PATH=%~dp0
SET PATH=%SKIA_M63_BIN_PATH%;C:\Python27;%PATH%

REM %comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"" x86

CD ../

CMD /K