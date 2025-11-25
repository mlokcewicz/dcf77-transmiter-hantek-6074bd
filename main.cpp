#include <windows.h>
#include <iostream>
#include <cstdint>
#include <sstream>
#include <iomanip>

#ifdef DLL_API
#undef DLL_API
#endif
#define DLL_API extern "C" __declspec(dllimport)

#include "DefMacro.h"
#include "HTSoftDll.h"
#include "HTHardDll.h"
#include "MeasDll.h"

//------------------------------------------------------------------------------

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

#define DCF77_GET_BITS(frame, bit_pos, mask) ((((((uint16_t)frame[((bit_pos) / 8) + 1] << 8) | (uint16_t)frame[(bit_pos) / 8]) >> ((bit_pos) % 8)) & (mask)))

#define DCF77_DECODER_FRAME_GET_FRAME_START(frame)          DCF77_GET_BITS(frame, 0, 0x01)
#define DCF77_DECODER_FRAME_GET_WEATHER_INFO(frame)         (((DCF77_GET_BITS(frame, 1, 0x7F)) | (DCF77_GET_BITS(frame, 8, 0x3F) << 7)))
#define DCF77_DECODER_FRAME_GET_AUX_ANTENNA(frame)          DCF77_GET_BITS(frame, 15, 0x01)
#define DCF77_DECODER_FRAME_GET_TIME_CHANGE_ANN(frame)      DCF77_GET_BITS(frame, 16, 0x01)
#define DCF77_DECODER_FRAME_GET_WINTER_TIME(frame)          DCF77_GET_BITS(frame, 17, 0x03)
#define DCF77_DECODER_FRAME_GET_LEAP_SECOND(frame)          DCF77_GET_BITS(frame, 19, 0x01)
#define DCF77_DECODER_FRAME_GET_TIME_START(frame)           DCF77_GET_BITS(frame, 20, 0x01)

#define DCF77_DECODER_FRAME_GET_MINUTES_UNITS(frame)        DCF77_GET_BITS(frame, 21, 0x0F)
#define DCF77_DECODER_FRAME_GET_MINUTES_TENS(frame)         DCF77_GET_BITS(frame, 25, 0x07)
#define DCF77_DECODER_FRAME_GET_MINUTES_PARITY(frame)       DCF77_GET_BITS(frame, 28, 0x01)

#define DCF77_DECODER_FRAME_GET_HOURS_UNITS(frame)          DCF77_GET_BITS(frame, 29, 0x0F)
#define DCF77_DECODER_FRAME_GET_HOURS_TENS(frame)           DCF77_GET_BITS(frame, 33, 0x03)
#define DCF77_DECODER_FRAME_GET_HOURS_PARITY(frame)         DCF77_GET_BITS(frame, 35, 0x01)

#define DCF77_DECODER_FRAME_GET_DAY_UNITS(frame)            DCF77_GET_BITS(frame, 36, 0x0F)
#define DCF77_DECODER_FRAME_GET_DAY_TENS(frame)             DCF77_GET_BITS(frame, 40, 0x03)
#define DCF77_DECODER_FRAME_GET_WEEKDAY(frame)              DCF77_GET_BITS(frame, 42, 0x07)
#define DCF77_DECODER_FRAME_GET_MONTH_UNITS(frame)          DCF77_GET_BITS(frame, 45, 0x0F)
#define DCF77_DECODER_FRAME_GET_MONTH_TENS(frame)           DCF77_GET_BITS(frame, 49, 0x01)
#define DCF77_DECODER_FRAME_GET_YEAR_UNITS(frame)           DCF77_GET_BITS(frame, 50, 0x0F)
#define DCF77_DECODER_FRAME_GET_YEAR_TENS(frame)            DCF77_GET_BITS(frame, 54, 0x0F)
#define DCF77_DECODER_FRAME_GET_DATE_PARITY(frame)          DCF77_GET_BITS(frame, 58, 0x01)

//------------------------------------------------------------------------------

const uint64_t TEST_DCF77_FRAME = 0b00101001011100000010100010010010001000100110010001101001000;
const unsigned int INITIAL_ERROR_TIME_MS    = 3000;
const unsigned int INITIAL_FRAME_START_MS   = 1800;
const unsigned int BIT_0_PULSE_MS           = 100;
const unsigned int BIT_1_PULSE_MS           = 200;
const unsigned int BIT_TOTAL_MS             = 1000;
const unsigned int MINUTE_MARKER_MS         = 850;

const float CARIER_FREQUENCY_HZ             = 77500.0f; 
const unsigned int AMPLITUDE_LOW            = 50;    
const unsigned int AMPLITUDE_HIGH           = 1500;  

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

static void modulate_dcf77(WORD dev, uint64_t dcf_frame)
{
    // Initial idle: generator OFF for 3 s - force receiver to enter error state
    p_ddsSetOnOff(dev, 0);
    Sleep(INITIAL_ERROR_TIME_MS);

    // Start frame
    p_ddsSetOnOff(dev, 1);
    Sleep(INITIAL_FRAME_START_MS);

    while (true)
    {
        // Process bits from MSB (bit 58) to LSB (bit 0)
        for (int8_t bit_idx = 58; bit_idx >= 0; bit_idx--)
        {
            uint8_t bit_value = (dcf_frame >> bit_idx) & 1u;

            uint16_t pulse_duration_ms   = (bit_value == 0u) ? BIT_0_PULSE_MS : BIT_1_PULSE_MS;
            uint16_t silence_duration_ms = 970 - pulse_duration_ms;

            std::cout << "Transmitting bit " << static_cast<int>(bit_value)
                      << " (pulse " << pulse_duration_ms << " ms, silence "
                      << silence_duration_ms << " ms) bit idx : " << static_cast<int>(58 - bit_idx) << "\n";

            p_ddsSDKSetAmp(dev, AMPLITUDE_LOW);       
            Sleep(pulse_duration_ms);
            
            p_ddsSDKSetAmp(dev, AMPLITUDE_HIGH);       
            Sleep(silence_duration_ms);
        }

        std::cout << "Transmitting sync bit (silence " << MINUTE_MARKER_MS << " ms)\n";

        // After full 59-bit frame: extra minute marker
        Sleep(MINUTE_MARKER_MS);
    }
}

static std::string dcf77_frame_to_string(uint64_t frame_bits)
{
    uint8_t frame[9] = {0};

    for (int bit = 0; bit <= 58; ++bit)
    {
        if ((frame_bits >> (58 - bit)) & 1ULL)
        {
            frame[bit / 8] |= static_cast<uint8_t>(1u << (bit % 8));
        }
    }

    uint8_t min_units = DCF77_DECODER_FRAME_GET_MINUTES_UNITS(frame);
    uint8_t min_tens  = DCF77_DECODER_FRAME_GET_MINUTES_TENS(frame);
    int minute = min_units + 10 * min_tens;

    uint8_t hour_units = DCF77_DECODER_FRAME_GET_HOURS_UNITS(frame);
    uint8_t hour_tens  = DCF77_DECODER_FRAME_GET_HOURS_TENS(frame);
    int hour = hour_units + 10 * hour_tens;

    uint8_t day_units = DCF77_DECODER_FRAME_GET_DAY_UNITS(frame);
    uint8_t day_tens  = DCF77_DECODER_FRAME_GET_DAY_TENS(frame);
    int day = day_units + 10 * day_tens;

    uint8_t month_units = DCF77_DECODER_FRAME_GET_MONTH_UNITS(frame);
    uint8_t month_tens  = DCF77_DECODER_FRAME_GET_MONTH_TENS(frame);
    int month = month_units + 10 * month_tens;

    uint8_t year_units = DCF77_DECODER_FRAME_GET_YEAR_UNITS(frame);
    uint8_t year_tens  = DCF77_DECODER_FRAME_GET_YEAR_TENS(frame);
    int year = 2000 + year_units + 10 * year_tens;

    std::ostringstream ss;
    ss << std::setfill('0')
       << year << "-"
       << std::setw(2) << month << "-"
       << std::setw(2) << day << " "
       << std::setw(2) << hour << ":"
       << std::setw(2) << minute;

    return ss.str();
}

//------------------------------------------------------------------------------

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

    rc = p_ddsSDKSetFre(dev, CARIER_FREQUENCY_HZ);   // 77.5 kHz
    std::cout << "ddsSDKSetFre rc = " << rc << "\n";

    rc = p_ddsSDKSetAmp(dev, AMPLITUDE_HIGH);
    std::cout << "ddsSDKSetAmp rc = " << rc << "\n";

    rc = p_ddsSDKSetOffset(dev, 0);
    std::cout << "ddsSDKSetOffset rc = " << rc << "\n";

    std::cout << "Ctrl-C to stop\n"; 

    std::cout << "Starting DCF77 modulation loop with date: " << dcf77_frame_to_string(TEST_DCF77_FRAME) << "... \n";

    modulate_dcf77(dev, TEST_DCF77_FRAME);

    // We never reach this point because of the infinite loop above.
    FreeLibrary(hHard);
    return 0; 
}
