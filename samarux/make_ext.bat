@echo off
rem Make a SamaruX external command
rem Usage: make_ext name
rem IE:    make_ext banner
cpm cc %1
cpm ccopt %1
zmac %1.zsm --rel
copy zout\%1.rel %1.rel
cpm2 link %1 [op]
if exist %1.x del %1.x
ren %1.prl %1.x
del %1.zsm
del %1.rel
del %1.sym