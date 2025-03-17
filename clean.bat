@echo off
echo -----------------------------------------------------------------------
echo Deleting lib
RD /S /Q ".\lib"
echo Deleting bin
RD /S /Q ".\bin"
echo Deleting projects
RD /S /Q ".\projects"
echo Done
echo on