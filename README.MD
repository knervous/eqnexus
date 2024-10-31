## Setting up the Project

This is a work in progress. Will be replacing vcpkg packages while finding the best balance of homegrown vs. third party.

1. Clone the repository with `git clone --recurse-submodules`
2. Run `external/vcpkg/bootstrap-vcpkg.bat`
3. Run `external/vcpkg/vcpkg install --triplet x86-windows` in the project root to install dependencies.
4. Build the project with VS2022
