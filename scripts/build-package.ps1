param (
    [string]$Version,
    [string]$CoreReleasePath = "core/Release",
    [string]$ProxyReleasePath = "proxy/Release",
    [string]$ResourcesFolder = "resources"
)

# Ensure required parameters are provided
if (-not $Version) {
    Write-Error "Version parameter is required."
    exit 1
}

# Define the zip filename
$ZipFilename = "eqnexus-$Version.zip"

# Step 1: Create version.json
$VersionFile = @{
    version = $Version
    filename = $ZipFilename
}
$VersionJson = ConvertTo-Json -InputObject $VersionFile -Depth 4
Set-Content -Path 'version.json' -Value $VersionJson -Encoding utf8
Write-Host "Created version.json"

# Step 2: Update config.ini
$ConfigPath = Join-Path -Path $ResourcesFolder -ChildPath 'config.ini'

function ConvertFrom-Ini {
    param ([string]$FilePath)
    $ini = @{}
    $currentSection = ''
    foreach ($line in Get-Content -Path $FilePath) {
        $line = $line.Trim()
        if ($line -match '^\[(.+)\]$') {
            $currentSection = $matches[1]
            $ini[$currentSection] = @{}
        } elseif ($line -match '^(.*?)=(.*)$') {
            $key, $value = $matches[1..2]
            $ini[$currentSection][$key.Trim()] = $value.Trim()
        }
    }
    return $ini
}

function ConvertTo-Ini {
    param ([hashtable]$Data)
    $output = ""
    foreach ($section in $Data.Keys) {
        $output += "[$section]`n"
        foreach ($key in $Data[$section].Keys) {
            $output += "$key=$($Data[$section][$key])`n"
        }
    }
    return $output
}

if (Test-Path -Path $ConfigPath) {
    $Config = ConvertFrom-Ini -FilePath $ConfigPath
} else {
    Write-Host "config.ini does not exist. Creating a new one."
    $Config = @{}
}

if (-not $Config["Version"]) {
    $Config["Version"] = @{}
}
$Config["Version"]["CoreVersion"] = $Version
$ConfigContent = ConvertTo-Ini -Data $Config
Set-Content -Path $ConfigPath -Value $ConfigContent -Encoding utf8
Write-Host "Updated config.ini with CoreVersion=$Version"

# Step 3: Create zip file
Add-Type -AssemblyName System.IO.Compression.FileSystem

# Create a temporary directory
$TempDir = Join-Path -Path ([System.IO.Path]::GetTempPath()) -ChildPath ([System.Guid]::NewGuid().ToString())
New-Item -ItemType Directory -Path $TempDir | Out-Null
# Copy files
Copy-Item -Path "$ProxyReleasePath\dinput8.dll" -Destination $TempDir
Copy-Item -Path "$ProxyReleasePath\dinput8.pdb" -Destination $TempDir
$EqNexusDir = Join-Path $TempDir "eqnexus"
New-Item -ItemType Directory -Path $EqNexusDir | Out-Null
Copy-Item -Path "$CoreReleasePath\core.dll" -Destination $EqNexusDir
Copy-Item -Path "$CoreReleasePath\core.pdb" -Destination $EqNexusDir
Copy-Item -Path "$ResourcesFolder\*" -Destination $EqNexusDir -Recurse

# Zip files
[System.IO.Compression.ZipFile]::CreateFromDirectory($TempDir, $ZipFilename)
Write-Host "Created zip file: $ZipFilename"

# Cleanup
Remove-Item -Path $TempDir -Recurse -Force
