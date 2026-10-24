$ErrorActionPreference = 'Stop'

$hhcCandidates = @(
    (Join-Path ${env:ProgramFiles(x86)} 'HTML Help Workshop\hhc.exe'),
    (Join-Path $env:ProgramFiles 'HTML Help Workshop\hhc.exe')
)
$hhc = $hhcCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1

if (-not $hhc) {
    throw 'HTML Help Workshop was not found. Install it from https://learn.microsoft.com/en-us/previous-versions/windows/desktop/htmlhelp/microsoft-html-help-downloads'
}

Push-Location $PSScriptRoot
try {
    $compilerOutput = & $hhc 'GraphDrawer.hhp' 2>&1
    $compilerOutput | ForEach-Object { Write-Host $_ }

    $chm = Join-Path $PSScriptRoot 'GraphDrawer.chm'
    if (($compilerOutput -join "`n") -notmatch '(?i)Created .+GraphDrawer\.chm' -or
        -not (Test-Path $chm) -or (Get-Item $chm).Length -eq 0) {
        throw "HTML Help Workshop did not produce a valid CHM file at '$chm'."
    }

    Write-Host "Built $chm"
}
finally {
    Pop-Location
}
