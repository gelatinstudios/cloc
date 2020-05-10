@echo off
cl cloc.c /MT /Od /Zi /Oi /link /incremental:no /opt:ref
IF %ERRORLEVEL% == 0 (
	cloc.exe
)
