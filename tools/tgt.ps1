#!/usr/bin/env pwsh
# tgt.ps1 <File-under-src> <fn> -- dump the target (delinked) asm for one function
param(
    [Parameter(Mandatory = $true)][string]$file,
    [Parameter(Mandatory = $true)][string]$fn
)
$ErrorActionPreference = 'Stop'
arm-none-eabi-objdump -dr "build/usa/delinks/src/$file.o" --disassemble="$fn" |
    Where-Object { $_ -match '^\s+[0-9a-f]+:|R_ARM_' }
