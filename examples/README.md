Building the examples
--

1. Get vcpkg: `git clone git@github.com:microsoft/vcpkg`
2. Bootstrap vcpkg: `cd vcpkg && ./bootstrap-vcpkg.bat`
3. Install dependencies: `vcpkg --triplet=x64-windows install portmidi portaudio qt boost`