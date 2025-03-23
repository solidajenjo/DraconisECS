@echo off
setlocal enabledelayedexpansion

echo Formatting C++ files...

:: Format all .cpp files
for /r ..\DraconisECS %%f in (*.cpp) do (
    echo Formatting %%f
    clang-format -i -style=file "%%f"
)

:: Format all .h files
for /r ..\DraconisECS %%f in (*.h) do (
    echo Formatting %%f
    clang-format -i -style=file "%%f"
)

:: Format all .hpp files (if any)
for /r ..\DraconisECS %%f in (*.hpp) do (
    echo Formatting %%f
    clang-format -i -style=file "%%f"
)

echo.
echo Code formatting complete!
echo. 