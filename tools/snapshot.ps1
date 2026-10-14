#!/usr/bin/env pwsh
# snapshot.ps1 <dir> -- copy the current build objects + report aside BEFORE editing
param([Parameter(Mandatory = $true)][string]$out)
$ErrorActionPreference = 'Stop'
$dst = Join-Path $out 'usa'
if (Test-Path $dst) { Remove-Item -Recurse -Force $dst }
New-Item -ItemType Directory -Force -Path $out | Out-Null
Copy-Item -Recurse 'build/usa/src' $dst
Copy-Item 'build/usa/report.json' (Join-Path $out 'usa.report.json')
Write-Output "snapshot -> $dst"
