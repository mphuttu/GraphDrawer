# Find hhc.exe
$hhc = "C:\Program Files (x86)\HTML Help Workshop\hhc.exe"
if (-not (Test-Path $hhc)) {
    $hhc = Get-ChildItem "C:\Program Files*" -Recurse -Filter "hhc.exe" -EA SilentlyContinue | Select-Object -First 1 -ExpandProperty FullName
}
Write-Host "hhc: $hhc"

# Compile CHM if hhc found
if ($hhc -and (Test-Path $hhc)) {
    Push-Location "d:\Projects\MFC\GraphDrawer\GraphDrawer\Help"
    & $hhc "GraphDrawer.hhp"
    $exitCode = $LASTEXITCODE
    Pop-Location
    Write-Host "hhc exit code: $exitCode"
    Test-Path "d:\Projects\MFC\GraphDrawer\GraphDrawer\Help\GraphDrawer.chm"
} else {
    Write-Host "hhc.exe not found!"
    $false
}
