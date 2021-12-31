@echo --------------------------------------------------------
@echo Build map parser wrapper

@cd..
@if not exist bin mkdir bin
@if not exist buildwin_converter mkdir buildwin_converter
@cd buildwin_converter
@del *.* /Q

@set outputPath=
@rem  /EHsc due to C++ exception stuff. add  -WX for warnings as errors
@rem Compile with no linking /c
@set compilerFlags=-nologo -MT -W4 -wd4100 -wd4201 -wd4189 -wd4505 /Zi -Oi /EHsc /c
@set compilerDefines=/DPARANOID=1 /DGLFW_USE_HYBRID_HPG

@set includeDir=/I../src/h/

@set compIn1=../converter/c/map_converter.c
@set compIn2=

@set linkStr=/link
@set linkInputA=user32.lib opengl32.lib Gdi32.lib shell32.lib ../buildwin_lib/maplib.lib

@cl %includeDir% %compilerFlags% %compilerDefines% %outputPath% %compIn1% %compIn2% %linkStr% %linkInputA%
@lib -nologo /out:mapconverter.lib map_converter.obj

@set compilerFlags=
@set compilerDefines=
@set includeDir=
@set compIn1=
@set compIn2=
@set linkStr=
@set linkInputA=

@cd..
@cd buildwin

