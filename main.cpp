#include <windows.h>
#include <iostream>
#include <cstdint>

#ifdef DLL_API
#undef DLL_API
#endif
#define DLL_API extern "C" __declspec(dllimport)

#include "DefMacro.h"
#include "HTSoftDll.h"
#include "HTHardDll.h"
#include "MeasDll.h"

// Scope / hardware
typedef WORD (WINAPI *PFN_dsoHTSearchDevice)(short *pDevInfo);
typedef WORD (WINAPI *PFN_dsoHTDeviceConnect)(WORD nDeviceIndex);
typedef WORD (WINAPI *PFN_dsoInitHard)(WORD nDeviceIndex);

// DDS / generator
typedef WORD (WINAPI *PFN_ddsSDKSetWaveType)(WORD nDeviceIndex, WORD nWaveType);
typedef WORD (WINAPI *PFN_ddsSDKSetFre)(WORD nDeviceIndex, float fFre);
typedef WORD (WINAPI *PFN_ddsSDKSetAmp)(WORD nDeviceIndex, WORD nAmp);
typedef WORD (WINAPI *PFN_ddsSDKSetOffset)(WORD nDeviceIndex, short nOffset);
typedef WORD (WINAPI *PFN_ddsSetOnOff)(WORD nDeviceIndex, WORD nOnOff);

static PFN_dsoHTSearchDevice  p_dsoHTSearchDevice  = nullptr;
static PFN_dsoHTDeviceConnect p_dsoHTDeviceConnect = nullptr;
static PFN_dsoInitHard        p_dsoInitHard        = nullptr;

static PFN_ddsSDKSetWaveType  p_ddsSDKSetWaveType  = nullptr;
static PFN_ddsSDKSetFre       p_ddsSDKSetFre       = nullptr;
static PFN_ddsSDKSetAmp       p_ddsSDKSetAmp       = nullptr;
static PFN_ddsSDKSetOffset    p_ddsSDKSetOffset    = nullptr;
static PFN_ddsSetOnOff        p_ddsSetOnOff        = nullptr;

#define LOAD_FUNC(h, name)                                                     \
    do {                                                                       \
        p_##name = reinterpret_cast<PFN_##name>(GetProcAddress(h, #name));     \
        if (!p_##name) {                                                       \
            std::cerr << "Missing function " #name " in DLL (GetLastError="    \
                      << GetLastError() << ")\n";                              \
            FreeLibrary(h);                                                    \
            return 1;                                                          \
        }                                                                      \
    } while (0)

static void modulate_dcf77(uint64_t dcf_frame, WORD dev)
{
    // Initial idle: generator OFF for 3 s
    p_ddsSetOnOff(dev, 0);
    Sleep(3000);

    // Pre-sync: generator ON for 1.8 s
    p_ddsSetOnOff(dev, 1);
    Sleep(1800);

    // Keep ON (same as your last gpio_set(true))
    // p_ddsSetOnOff(dev, 1);

    while (true)
    {
        // Process bits from MSB (bit 58) to LSB (bit 0)
        for (int8_t bit_idx = 58; bit_idx >= 0; bit_idx--)
        {
            uint8_t bit_value = (dcf_frame >> bit_idx) & 1u;

            // Pulse duration: 120 ms for 0, 220 ms for 1
            uint16_t pulse_duration_ms   = (bit_value == 0u) ? 100 : 200;
            // Total "bit window" ~980 ms (your tuned value)
            uint16_t silence_duration_ms = 970 - pulse_duration_ms;

            // "Pulse": generator OFF
            // p_ddsSetOnOff(dev, 0);
            p_ddsSDKSetAmp(dev, 50);       // adjust empirically
            Sleep(pulse_duration_ms);
            
            // "Silence": generator ON
            // p_ddsSetOnOff(dev, 1);
            p_ddsSDKSetAmp(dev, 1500);       // adjust empirically
            Sleep(silence_duration_ms);
        }

        // After full 59-bit frame: extra 900 ms ON (minute marker)
        // p_ddsSetOnOff(dev, 1);
        // p_ddsSDKSetAmp(dev, 1500);       // adjust empirically
        Sleep(850u);
    }
}

int main()
{
    std::cout << "Hantek DCF77 generator\n";

    HMODULE hHard = LoadLibraryA("HTHardDll.dll");
    if (!hHard)
    {
        DWORD err = GetLastError();
        std::cerr << "Cannot load HTHardDll.dll, GetLastError = "
                  << err << "\n";
        std::cerr << "Make sure HTHardDll.dll is next to the executable "
                     "and has matching architecture.\n";
        return 1;
    }

    LOAD_FUNC(hHard, dsoHTSearchDevice);
    LOAD_FUNC(hHard, dsoHTDeviceConnect);
    LOAD_FUNC(hHard, dsoInitHard);

    LOAD_FUNC(hHard, ddsSDKSetWaveType);
    LOAD_FUNC(hHard, ddsSDKSetFre);
    LOAD_FUNC(hHard, ddsSDKSetAmp);
    LOAD_FUNC(hHard, ddsSDKSetOffset);
    LOAD_FUNC(hHard, ddsSetOnOff);

    short devInfo[32] = {0};
    WORD rc = p_dsoHTSearchDevice(devInfo);

    if (rc != HT_OK)
    {
        std::cerr << "dsoHTSearchDevice rc = " << rc << " (error)\n";
        FreeLibrary(hHard);
        return 1;
    }

    int devCount = 0;
    for (int i = 0; i < 32; ++i)
    {
        if (devInfo[i] != 0)
            ++devCount;
    }

    std::cout << "Found: " << devCount << " devices\n"; 
    if (devCount == 0)
    {
        FreeLibrary(hHard);
        return 0;
    }

    WORD dev = 0; // first device

    rc = p_dsoHTDeviceConnect(dev);
    std::cout << "dsoHTDeviceConnect rc = " << rc << "\n";
    if (rc != HT_OK)
    {
        std::cerr << "DeviceConnect fail rc=" << rc << "\n";
        FreeLibrary(hHard);
        return 1;
    }

    rc = p_dsoInitHard(dev);
    std::cout << "dsoInitHard rc = " << rc << "\n";
    if (rc != HT_OK)
    {
        std::cerr << "InitHard fail rc=" << rc << "\n";
        FreeLibrary(hHard);
        return 1;
    }

    rc = p_ddsSDKSetWaveType(dev, WAVE_SINE);
    std::cout << "ddsSDKSetWaveType rc = " << rc << "\n";

    rc = p_ddsSDKSetFre(dev, 77500.0f);   // 77.5 kHz
    std::cout << "ddsSDKSetFre rc = " << rc << "\n";

    rc = p_ddsSDKSetAmp(dev, 1500);       // adjust empirically
    std::cout << "ddsSDKSetAmp rc = " << rc << "\n";

    rc = p_ddsSDKSetOffset(dev, 0);
    std::cout << "ddsSDKSetOffset rc = " << rc << "\n";

    // Example DCF77 frame (fill with your real 59-bit frame)
    // NOTE: use a valid 59-bit frame value here.
    uint64_t dcf_frame = 0b00101001011100000010100010010010001000100110010001101001000;

    modulate_dcf77(dcf_frame, dev);

    // We never reach this point because of the infinite loop above.
    FreeLibrary(hHard);
    return 0; 
}
