$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

Push-Location $PSScriptRoot
try {
    & .\create_deployment.bat
    if ($LASTEXITCODE -ne 0) {
        throw 'Could not assemble the release folder. Build Release | x86 first.'
    }

    $releaseFolder = Join-Path $PSScriptRoot 'deploy\OxyWare'
    $archiveName = 'OxyWare-CS16-8684-win32.zip'
    $archivePath = Join-Path $PSScriptRoot "deploy\$archiveName"
    $expectedFiles = @(
        'OxyWare.exe', 'cheater.dll', 'oxui.dll', 'util.dll',
        'msvcp140.dll', 'vcruntime140.dll', 'README.txt', 'LICENSE'
    )

    foreach ($file in $expectedFiles) {
        if (-not (Test-Path -LiteralPath (Join-Path $releaseFolder $file) -PathType Leaf)) {
            throw "Missing release file: $file"
        }
    }

    $unexpectedFiles = Get-ChildItem -LiteralPath $releaseFolder -File |
        Where-Object { $_.Name -notin $expectedFiles }
    if ($unexpectedFiles) {
        throw "Unexpected files in release folder: $($unexpectedFiles.Name -join ', ')"
    }

    Compress-Archive -LiteralPath $releaseFolder -DestinationPath $archivePath -Force
    $checksum = (Get-FileHash -LiteralPath $archivePath -Algorithm SHA256).Hash
    $checksumPath = "$archivePath.sha256"
    Set-Content -LiteralPath $checksumPath -Value "$checksum  $archiveName" -Encoding ascii

    Write-Output "Release archive: $archivePath"
    Write-Output "SHA-256: $checksum"
}
finally {
    Pop-Location
}
