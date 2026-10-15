#!/usr/bin/env pwsh
# fndiff.ps1 <unit-under-src> <function> [usa|jp]
#   e.g. ./tools/fndiff.ps1 Interface/Menu/Shop/Shop_boardU func_ov043_020bca50
# Rebuilds build/<region>/src/<unit>.o, then prints a normalised objdump diff
# (target left, current right). Prints "MATCH <fn>" when identical.
# Normalisation drops instruction addresses, branch-target labels/addresses and
# literal-pool comments, and re-writes unresolved-bl placeholders.
param(
    [Parameter(Mandatory = $true)][string]$unit,
    [Parameter(Mandatory = $true)][string]$fn,
    [string]$region = 'usa'
)

$ErrorActionPreference = 'Stop'
ninja "build/$region/src/$unit.o" 2>&1 | Out-Host
if ($LASTEXITCODE -ne 0) { Write-Output "build failed (is $region configured?)"; exit 2 }

function Get-Norm($path) {
    arm-none-eabi-objdump -dr $path --disassemble="$fn" |
        Select-String -Pattern '^\s+[0-9a-f]+:' |
        ForEach-Object {
            $l = ($_.Line -replace '^\s+[0-9a-f]+:\s*', '') -replace '\s+$', ''
            # drop reloc/address comments and named branch targets
            $l = ($l -replace '@\s*[0-9a-f]+.*$', '').TrimEnd()
            $l = $l -replace '<[^>]*>', '<sym>'
            # branch operands are label addresses -> drop them
            $l = $l -replace '\b(b\w*|blx?)\s+[0-9a-f]+\s+<sym>', '$1 <sym>'
            # unresolved-bl placeholder (ours) vs delinker-resolved ebfffffe
            $l = $l -replace '^eb[0-9a-f]{6}\s+bl\s+<sym>$', 'bl <sym>'
            ($l -replace '\s+$', '')
        }
}

$a = Get-Norm "build/$region/delinks/src/$unit.o"
$b = Get-Norm "build/$region/src/$unit.o"

# NOTE: Compare-Object has *set* semantics (it silently ignores reordering), so the
# MATCH check must be a positional comparison of the two arrays.
$diffs = @()
$max = [Math]::Max($a.Count, $b.Count)
for ($i = 0; $i -lt $max; $i++) {
    $t = if ($i -lt $a.Count) { $a[$i] } else { '' }
    $c = if ($i -lt $b.Count) { $b[$i] } else { '' }
    if ($t -ne $c) { $diffs += , @($i, $t, $c) }
}
if ($diffs.Count -eq 0) {
    Write-Output "MATCH $fn"
    exit 0
}
Write-Output ("{0} differing rows (target left, current right):" -f $diffs.Count)
foreach ($d in $diffs) {
    Write-Output ("* {0,-3} {1,-46} | {2}" -f $d[0], $d[1], $d[2])
}
