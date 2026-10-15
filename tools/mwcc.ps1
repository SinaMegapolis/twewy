#!/usr/bin/env pwsh
# mwcc.ps1 -src <file.c> -obj <out.o> [-region usa|jp]
param(
    [Parameter(Mandatory = $true)][string]$src,
    [Parameter(Mandatory = $true)][string]$obj,
    [string]$region = 'usa'
)
$dir = Split-Path -Parent $obj
if ($dir) { New-Item -ItemType Directory -Force $dir | Out-Null }
$flags = @(
    '-O4,p', '-enum', 'int', '-char', 'signed', '-proc', 'arm946e', '-gccext,on',
    '-fp', 'soft', '-inline', 'noauto', '-RTTI', 'off', '-interworking', '-w', 'off',
    '-sym', 'on', '-gccinc', '-nolink', '-msgstyle', 'gcc', '-enc', 'SJIS', '-ipa', 'file',
    '-str', 'noreuse', '-Cpp_exceptions', 'off',
    '-i', 'include', '-i', 'libs\c\include', '-i', 'libs\cpp\include',
    '-i', 'libs\cri\include', '-i', 'libs\nitro\include', '-i', 'libs\runtime\include',
    '-lang=c99', '-d', "REGION_$(if ($region -eq 'jp') {'JP'} else {'USA'})"
)
& .\tools\mwccarm\2.0\sp1p5\mwccarm.exe @flags -MD -c $src -o $obj
exit $LASTEXITCODE
