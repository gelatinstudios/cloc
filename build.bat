@echo off
cl cloc.c /MT /O2 /Oi /Zi /link /incremental:no /opt:ref
IF %ERRORLEVEL% == 0 (
	cloc.exe
)
