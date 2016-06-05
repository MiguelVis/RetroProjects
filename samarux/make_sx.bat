@echo off
rem Make the SamaruX shell and the built-in commands.
rem Usage: make_sx
if exist sx.com del sx.com
if exist sx.zsm del sx.zsm
if exist sx.hex del sx.hex
if exist sx.prn del sx.prn
cpm cc sx
cpm ccopt sx
cpm zsm sx
cpm hextocom sx
del sx.zsm
del sx.hex
rem Make the linked symbols header
cpm prntosym sx.prn ">" sx.sym
echo #asm > sym_header
echo #endasm > sym_footer
copy /Y sym_header + sx.sym + sym_footer sx_sym.h
del sym_header
del sym_footer
del sx.prn
del sx.sym
echo.
echo -------------
echo Build results
echo -------------
echo.
call make_tst sx.com
call make_tst sx_sym.h
echo.
echo Remember to modify the file samarux.h if needed.
echo.
echo Remember to re-build the manual if needed, with:
echo.
echo cpm sx batch mk_doc.sx dest_file formfeed version date
echo.