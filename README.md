# hantek-dcf77-generator
DCF77 signal generator based on **Hantek 6074BD** oscilloscope SDK (`HTHardDll`, `HTSoftDll`, `HTDisplayDll`, `MeasDll`) on **Windows** with **MinGW** and **CMake**.

**Capabilities:**
- Automatic DLL loading with `LoadLibrary`
- Device discovery (`dsoHTSearchDevice`)
- Device connection (`dsoHTDeviceConnect`)
- Hardware initialization (`dsoInitHard`)
- DCF77 carier generation with selected time frame

**Hardware:**
- Hantek 6074BD USB Oscilloscope

## Tools
- MinGW-w64 (32-bit or 64-bit depending on DLL set)
- CMake ≥ 3.10
- Hantek SDK (DLL + headers)

## Build

### Generate ninja files
```sh
cmake -S . -B build -G "MinGW Makefiles"
```

### Build project
```sh
cmake --build build -- -j
```

### Run
```sh
./build/HantekDCF77Generator.exe 
```
