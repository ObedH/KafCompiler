
set shell := ["powershell", "-c"]

build:
	C:\mingw64\bin\gcc.exe -std=c17 -Wall -Wpedantic -Iinclude src\*.c -o build\Debug\KafCompiler.exe

clean:
	if (Test-Path build\Debug\Perchrit.exe) { Remove-Item build\Debug\KafCompiler.exe }

run:
	build\Debug\KafCompiler.exe
