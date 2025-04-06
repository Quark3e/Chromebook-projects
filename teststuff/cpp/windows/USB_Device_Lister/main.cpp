/**
 * @file main.cpp
 * @author "Marc"
 * @brief source: https://cboard.cprogramming.com/windows-programming/114294-getting-list-usb-devices-listed-system-post851132.html#post851132
 * @details This program lists all USB devices connected to the system and displays their information.
 * It uses the Windows API to retrieve the device list and their details.
 * @version 0.1
 * @date 04-04-2009
 * 
 * 
 */

#include <windows.h>
#include <iostream>
 
// Namespace
using namespace std;
 
// Main
int main()
{
    // Program
    cout << "USB Device Lister." << endl;
 
    // Get Number Of Devices
    UINT nDevices = 0;
    GetRawInputDeviceList( NULL, &nDevices, sizeof( RAWINPUTDEVICELIST ) );
 
    // Got Any?
    if( nDevices < 1 )
    {
        // Exit
        cout << "ERR: 0 Devices?";
        cin.get();
        return 0;
    }
     
    // Allocate Memory For Device List
    PRAWINPUTDEVICELIST pRawInputDeviceList;
    pRawInputDeviceList = new RAWINPUTDEVICELIST[ sizeof( RAWINPUTDEVICELIST ) * nDevices ];
 
    // Got Memory?
    if( pRawInputDeviceList == NULL )
    {
        // Error
        cout << "ERR: Could not allocate memory for Device List.";
        cin.get();
        return 0;
    }
     
    // Fill Device List Buffer
    int nResult;
    nResult = GetRawInputDeviceList( pRawInputDeviceList, &nDevices, sizeof( RAWINPUTDEVICELIST ) );
 
    // Got Device List?
    if( nResult < 0 )
    {
        // Clean Up
        delete [] pRawInputDeviceList;
 
        // Error
        cout << "ERR: Could not get device list.";
        cin.get();
        return 0;
    }
 
    // Loop Through Device List
    for( UINT i = 0; i < nDevices; i++ )
    {
        // Get Character Count For Device Name
        UINT nBufferSize = 0;
        nResult = GetRawInputDeviceInfo( pRawInputDeviceList[i].hDevice, // Device
                                         RIDI_DEVICENAME,                // Get Device Name
                                         NULL,                           // NO Buff, Want Count!
                                         &nBufferSize );                 // Char Count Here!
 
        // Got Device Name?
        if( nResult < 0 )
        {
            // Error
            cout << "ERR: Unable to get Device Name character count.. Moving to next device." << endl << endl;
 
            // Next
            continue;
        }
 
        // Allocate Memory For Device Name
        WCHAR* wcDeviceName = new WCHAR[ nBufferSize + 1 ];
         
        // Got Memory
        if( wcDeviceName == NULL )
        {
            // Error
            cout << "ERR: Unable to allocate memory for Device Name.. Moving to next device." << endl << endl;
 
            // Next
            continue;
        }
 
        // Get Name
        nResult = GetRawInputDeviceInfo( pRawInputDeviceList[i].hDevice, // Device
                                         RIDI_DEVICENAME,                // Get Device Name
                                         wcDeviceName,                   // Get Name!
                                         &nBufferSize );                 // Char Count
 
        // Got Device Name?
        if( nResult < 0 )
        {
            // Error
            cout << "ERR: Unable to get Device Name.. Moving to next device." << endl << endl;
 
            // Clean Up
            delete [] wcDeviceName;
 
            // Next
            continue;
        }
 
        // Set Device Info & Buffer Size
        RID_DEVICE_INFO rdiDeviceInfo;
        rdiDeviceInfo.cbSize = sizeof( RID_DEVICE_INFO );
        nBufferSize = rdiDeviceInfo.cbSize;
 
        // Get Device Info
        nResult = GetRawInputDeviceInfo( pRawInputDeviceList[i].hDevice,
                                         RIDI_DEVICEINFO,
                                         &rdiDeviceInfo,
                                         &nBufferSize );
 
        // Got All Buffer?
        if( nResult < 0 )
        {
            // Error
            cout << "ERR: Unable to read Device Info.. Moving to next device." << endl << endl;
 
            // Next
            continue;
        }
 
        // Mouse
        if( rdiDeviceInfo.dwType == RIM_TYPEMOUSE )
        {
            // Current Device
            cout << endl << "Displaying device " << i+1 << " information. (MOUSE)" << endl;
            wcout << L"Device Name: " << wcDeviceName << endl;
            cout << "Mouse ID: " << rdiDeviceInfo.mouse.dwId << endl;
            cout << "Mouse buttons: " << rdiDeviceInfo.mouse.dwNumberOfButtons << endl;
            cout << "Mouse sample rate (Data Points): " << rdiDeviceInfo.mouse.dwSampleRate << endl;
            if( rdiDeviceInfo.mouse.fHasHorizontalWheel )
            {
                cout << "Mouse has horizontal wheel" << endl;
            }
            else
            {
                cout << "Mouse does not have horizontal wheel" << endl;
            }
        }
 
        // Keyboard
        else if( rdiDeviceInfo.dwType == RIM_TYPEKEYBOARD )
        {
            // Current Device
            cout << endl << "Displaying device " << i+1 << " information. (KEYBOARD)" << endl;
            wcout << L"Device Name: " << wcDeviceName << endl;
            cout << "Keyboard mode: " << rdiDeviceInfo.keyboard.dwKeyboardMode << endl;
            cout << "Number of function keys: " << rdiDeviceInfo.keyboard.dwNumberOfFunctionKeys << endl;
            cout << "Number of indicators: " << rdiDeviceInfo.keyboard.dwNumberOfIndicators << endl;
            cout << "Number of keys total: " << rdiDeviceInfo.keyboard.dwNumberOfKeysTotal << endl;
            cout << "Type of the keyboard: " << rdiDeviceInfo.keyboard.dwType << endl;
            cout << "Subtype of the keyboard: " << rdiDeviceInfo.keyboard.dwSubType << endl;
        }
 
        // Some HID
        else // (rdi.dwType == RIM_TYPEHID)
        {
            // Current Device
            cout << endl << "Displaying device " << i+1 << " information. (HID)" << endl;
            wcout << L"Device Name: " << wcDeviceName << endl;
            cout << "Vendor Id:" << rdiDeviceInfo.hid.dwVendorId << endl;
            cout << "Product Id:" << rdiDeviceInfo.hid.dwProductId << endl;
            cout << "Version No:" << rdiDeviceInfo.hid.dwVersionNumber << endl;
            cout << "Usage for the device: " << rdiDeviceInfo.hid.usUsage << endl;
            cout << "Usage Page for the device: " << rdiDeviceInfo.hid.usUsagePage << endl;
        }
 
        // Delete Name Memory!
        delete [] wcDeviceName;
    }
 
    // Clean Up - Free Memory
    delete [] pRawInputDeviceList;
 
    // Exit
    cout << endl << "Finnished.";
    cin.get();
    return 0;
}