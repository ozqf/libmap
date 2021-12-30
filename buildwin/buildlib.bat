@echo --------------------------------------------------------
@echo Build Qodot map parser library

@cd..
@if not exist bin mkdir bin
@if not exist buildwin_lib mkdir buildwin_lib
@cd buildwin_lib
@del *.* /Q

@set outputPath=/Femaplib.exe
@rem  /EHsc due to C++ exception stuff. add  -WX for warnings as errors
@rem Compile with no linking /c
@set compilerFlags=-nologo -MT -W4 -wd4100 -wd4201 -wd4189 -wd4505 /Zi -Oi /EHsc /c
@set compilerDefines=/DPARANOID=1 /DGLFW_USE_HYBRID_HPG

@set includeDir=/I../src/h/

@set compIn1=../src/c/geo_generator.c ../src/c/map_data.c ../src/c/map_parser.c
@set compIn2=../src/c/matrix.c ../src/c/surface_gatherer.c ../src/c/vector.c

@set linkStr=/link
@set linkInputA=user32.lib opengl32.lib Gdi32.lib shell32.lib

@cl %includeDir% %compilerFlags% %compilerDefines% %outputPath% %compIn1% %compIn2% %linkStr% %linkInputA%
@lib -nologo /out:maplib.lib geo_generator.obj map_data.obj map_parser.obj matrix.obj surface_gatherer.obj vector.obj

@set compilerFlags=
@set compilerDefines=
@set includeDir=
@set compIn1=
@set compIn2=
@set linkStr=
@set linkInputA=

@cd..
@cd buildwin

