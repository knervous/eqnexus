# EQ Nexus

EQ Nexus is an EQEmu custom server file, configuration and patch manager that runs inside of EQ. It allows for streamlined patch updates to servers and custom files per server with one installation of RoF2 EQ rather than 1 installation per server.

## EQ Players

To use EQ Nexus, go to the Releases section and download the latest release zip. Extract this in your base RoF2 directory (only RoF2 is supported!) and you're good to go!

## Server Operators

To have your server with custom content (or configuration) added, take a look at the server_manifest.json and submit a merge request with your server's metadata. There are several ways to host your custom content. The easiest would be forking this repo (https://github.com/knervous/requiemfiles) and replacing all the content under `rof` with whatever you'd like, and changing the data in the `manifest.json` to match your server's information. Please reach out to temp0 in the EQEmu discord under #project-requiem for help. Onboarding should be a simple process and updating files after the fact should be streamlined.

## Local Development

This is a work in progress. Will be replacing vcpkg packages while finding the best balance of homegrown vs. third party.

1. Clone the repository with `git clone --recurse-submodules`
2. Run `external/vcpkg/bootstrap-vcpkg.bat`
3. Run `external/vcpkg/vcpkg install --triplet x86-windows-static` in the project root to install dependencies.
4. Build the project with VS2022
5. Create a folder eqnexus in your rof2 folder
5. HotReload.ps1 has a copy command that copies build output into your rof2 folder, replace the file in the line `$coreDllDestination = "$PSScriptRoot\..\everquest_rof2\eqnexus\core.dll"` to your rof2 directory and each build will copy files over automatically
