# SQLite3 amalgamation 다운로드 스크립트
# 실행: powershell -ExecutionPolicy Bypass -File download_sqlite.ps1

$outDir  = "$PSScriptRoot\third_party\sqlite"
$zipUrl  = "https://www.sqlite.org/2024/sqlite-amalgamation-3450300.zip"
$zipFile = "$env:TEMP\sqlite3.zip"

Write-Host "SQLite3 amalgamation 다운로드 중..." -ForegroundColor Cyan

New-Item -ItemType Directory -Force -Path $outDir | Out-Null

Invoke-WebRequest -Uri $zipUrl -OutFile $zipFile
Expand-Archive  -Path $zipFile -DestinationPath "$env:TEMP\sqlite3_tmp" -Force

$extracted = Get-ChildItem "$env:TEMP\sqlite3_tmp" -Recurse -Filter "sqlite3.h"  | Select-Object -First 1
Copy-Item $extracted.FullName                                  "$outDir\sqlite3.h"
Copy-Item ($extracted.DirectoryName + "\sqlite3.c")            "$outDir\sqlite3.c"

Remove-Item $zipFile -Force
Remove-Item "$env:TEMP\sqlite3_tmp" -Recurse -Force

Write-Host "완료: $outDir" -ForegroundColor Green
Write-Host ""
Write-Host "다음 단계:" -ForegroundColor Yellow
Write-Host "  Visual Studio > 프로젝트 속성 > C/C++ > 전처리기 정의에 ENABLE_SQLITE 추가"
Write-Host "  또는 vcxproj 의 PreprocessorDefinitions 에 ENABLE_SQLITE; 를 삽입하세요."
