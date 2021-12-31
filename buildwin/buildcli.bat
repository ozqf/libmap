@echo --------------------------------------------------------
@echo Build Qodot map parser CLI exe

@cd..
@if not exist bin mkdir bin
@if not exist buildwin_cli mkdir buildwin_cli
@cd buildwin_cli
@del *.* /Q

@set outputPath=/Fe../bin/maplibcli.exe
@rem  /EHsc due to C++ exception stuff. add  -WX for warnings as errors
@rem Compile with no linking /c
@rem add /O2 and -Oi for optimsation
@set compilerFlags=-nologo -MT -W4 -wd4100 -wd4201 -wd4189 -wd4505 /Zi -Oi /EHsc
@set compilerDefines=/DPARANOID=1 /DGLFW_USE_HYBRID_HPG

@set includeDir=/I../src/h/

@set compIn1=../cli/main.c
@set compIn2=

@set linkStr=/link
@set linkInputA=user32.lib opengl32.lib Gdi32.lib shell32.lib
@set linkInputB= ../buildwin_lib/maplib.lib ../buildwin_converter/mapconverter.lib

@cl %includeDir% %compilerFlags% %compilerDefines% %outputPath% %compIn1% %compIn2% %linkStr% %linkInputA% %linkInputB%

@set compilerFlags=
@set compilerDefines=
@set includeDir=
@set compIn1=
@set compIn2=
@set linkStr=
@set linkInputA=
@set linkInputB=

@cd..
@cd buildwin
