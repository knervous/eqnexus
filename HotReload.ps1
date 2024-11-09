param (
    [string]$Config = "Debug"
)

# Define the events and delay
$unloadEventName = "Global\UnloadEvent"
$reloadEventName = "Global\ReloadEvent"
$copyDelayMs = 50 # Delay between copy attempts
$maxAttempts = 20 # Maximum number of copy attempts

# Define paths for DLL and resources copy
$coreDllSource = "$PSScriptRoot\$Config\core.dll"
$coreDllDestination = "$PSScriptRoot\..\everquest_rof2\eqnexus\core.dll"
$resourcesSource = "$PSScriptRoot\resources"
$resourcesDestination = "$PSScriptRoot\..\everquest_rof2\eqnexus"

# Find the process ID of eqgame.exe
$process = Get-Process -Name "eqgame" -ErrorAction SilentlyContinue
if ($process -eq $null) {
    Write-Host "Process eqgame.exe not found."
    Write-Host "Attempting to copy core.dll (Attempt $($attempts + 1) of $maxAttempts)..."
    Copy-Item -Path $coreDllSource -Destination $coreDllDestination -Force
    Write-Host "core.dll copied successfully."
    Copy-Item -Path "$resourcesSource\*" -Destination $resourcesDestination -Recurse -Force
    Write-Host "Resources folder copied successfully."
    exit 0
}
$processId = $process.Id
Write-Host "Found eqgame.exe with PID: $processId"

# Open the UnloadEvent
$unloadEvent = [System.Threading.EventWaitHandle]::OpenExisting($unloadEventName)
if ($unloadEvent -eq $null) {
    Write-Host "Unload event $unloadEventName not found."
    exit 1
}

# Open the ReloadEvent
$reloadEvent = [System.Threading.EventWaitHandle]::OpenExisting($reloadEventName)
if ($reloadEvent -eq $null) {
    Write-Host "Reload event $reloadEventName not found."
    exit 1
}

# Trigger the UnloadEvent
Write-Host "Triggering UnloadEvent..."
$unloadEvent.Set()

# Attempt to copy the core.dll file and resources folder with retries
$attempts = 0
while ($attempts -lt $maxAttempts) {
    try {
        Write-Host "Attempting to copy core.dll (Attempt $($attempts + 1) of $maxAttempts)..."
        Copy-Item -Path $coreDllSource -Destination $coreDllDestination -Force
        Write-Host "core.dll copied successfully."

        Write-Host "Copying resources folder..."
        Copy-Item -Path "$resourcesSource\*" -Destination $resourcesDestination -Recurse -Force
        Write-Host "Resources folder copied successfully."
        break
    }
    catch {
        Write-Host "Failed to copy core.dll or resources folder. Retrying in $copyDelayMs ms..."
        Start-Sleep -Milliseconds $copyDelayMs
        $attempts++
    }
}

if ($attempts -ge $maxAttempts) {
    Write-Host "Failed to copy core.dll and resources after $maxAttempts attempts."
    exit 1
}

# Trigger the ReloadEvent
Write-Host "Triggering ReloadEvent..."
$reloadEvent.Set()

# Clean up
$unloadEvent.Close()
$reloadEvent.Close()
Write-Host "Events triggered and DLL and resources copied successfully."
