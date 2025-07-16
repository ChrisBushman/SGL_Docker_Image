@echo off
make
if errorlevel == 1 goto end
echo Created sound driver for SEGASATURN...
:end