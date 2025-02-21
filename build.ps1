$Root = $PSScriptRoot
$BuildPath = $Root + "\build"
$TargetProcessor = "cortex-m23"
$ToolchainFile = $Root + "\toolchain\armclang.cmake"
$PathToSource = $Root

$CmakeArgsArr = @(
    '-G "Ninja" --toolchain=' + $ToolchainFile
    '-B ' + $BuildPath
    '-S ' + $PathToSource
    '-DCMAKE_SYSTEM_PROCESSOR=' + $TargetProcessor
)

$CmakeArgsStr = [string]$CmakeArgsArr
$CmakeArgs = $CmakeArgsStr.Split(" ")

# Write-Output $CmakeArgs

& cmake $CmakeArgs
& cmake "--build $BuildPath --target bl2".Split(" ")