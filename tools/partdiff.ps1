#!/usr/bin/env pwsh
# partdiff.ps1 <partfile-basename> <function> [usa|jp]
#   e.g. ./tools/partdiff.ps1 Shop_p5 func_ov043_020af6b4
# Compiles src/Interface/Menu/<part>.c standalone into build/tmp/<part>.o and
# prints a normalised objdump diff against the target delinked Shop.o.
param(
    [Parameter(Mandatory = $true)][string]$part,
    [Parameter(Mandatory = $true)][string]$fn,
    [string]$region = 'usa'
)

$ErrorActionPreference = 'Stop'
$src = "src/Interface/Menu/$part.c"
$obj = "build/$region/tmp/$part.o"
New-Item -ItemType Directory -Force "build/$region/tmp" | Out-Null

$mwcc = ".\tools\mwccarm\2.0\sp1p5\mwccarm.exe"
$flags = @(
    '-O4,p', '-enum', 'int', '-char', 'signed', '-proc', 'arm946e', '-gccext,on',
    '-fp', 'soft', '-inline', 'noauto', '-RTTI', 'off', '-interworking', '-w', 'off',
    '-sym', 'on', '-gccinc', '-nolink', '-msgstyle', 'gcc', '-enc', 'SJIS', '-ipa', 'file',
    '-str', 'noreuse', '-Cpp_exceptions', 'off',
    '-i', 'include', '-i', 'libs\c\include', '-i', 'libs\cpp\include',
    '-i', 'libs\cri\include', '-i', 'libs\nitro\include', '-i', 'libs\runtime\include',
    '-lang=c99', "-d", "REGION_$(if ($region -eq 'jp') {'JP'} else {'USA'})"
)
& $mwcc @flags -MD -c $src -o $obj
if ($LASTEXITCODE -ne 0) { Write-Output "compile failed"; exit 2 }

function Get-Norm($path) {
    arm-none-eabi-objdump -dr $path --disassemble="$fn" |
        Select-String -Pattern '^\s+[0-9a-f]+:' |
        ForEach-Object {
            $l = ($_.Line -replace '^\s+[0-9a-f]+:\s*', '') -replace '\s+$', ''
            $l = ($l -replace '@\s*[0-9a-f]+.*$', '').TrimEnd()
            $l = $l -replace '<[^>]*>', '<sym>'
            $l = $l -replace '\b(b\w*|blx?)\s+[0-9a-f]+\s+<sym>', '$1 <sym>'
            $l = $l -replace '^eb[0-9a-f]{6}\s+bl\s+<sym>$', 'bl <sym>'
            ($l -replace '\s+$', '')
        }
}

$targetObj = "build/$region/delinks/src/Interface/Menu/Shop.o"
$a = Get-Norm $targetObj
$b = Get-Norm $obj
if ($a.Count -eq 0) { Write-Output "no target asm for $fn"; exit 3 }

$diffs = @()
$max = [Math]::Max($a.Count, $b.Count)
for ($i = 0; $i -lt $max; $i++) {
    $t = if ($i -lt $a.Count) { $a[$i] } else { '' }
    $c = if ($i -lt $b.Count) { $b[$i] } else { '' }
    if ($t -ne $c) { $diffs += , @($i, $t, $c) }
}
if ($diffs.Count -eq 0) { Write-Output "MATCH $fn"; exit 0 }
Write-Output ("{0} differing rows (target left, current right):" -f $diffs.Count)
foreach ($d in $diffs) {
    Write-Output ("* {0,-3} {1,-46} | {2}" -f $d[0], $d[1], $d[2])
}
