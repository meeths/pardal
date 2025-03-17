@echo off
echo -----------------------------------------------------------------------
echo [92mGenerating projects...[0m
tools\premake5\premake5 vs2022 --file=pardal.lua
echo [92mPardal code generation finished[0m