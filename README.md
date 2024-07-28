# set-date-in-windows-process

1. Clone this folder and place it in the Detours folder.

2. Open Visual Studio Tools\VC\x64 Native Tools Command Prompt
  - pushd "Detours Path"
  - pushd src
  - nmake all
  - popd
  - pushd set-date-in-windows-process-ms-nmake-ver
  - nmake all
  - exit

3. The executable file output is located at "Detours Path\bin.X64"

Run a Windows process under a specific date.

https://renenyffenegger.ch/notes/Windows/tools/event-hooking/set-date-in-process/index
