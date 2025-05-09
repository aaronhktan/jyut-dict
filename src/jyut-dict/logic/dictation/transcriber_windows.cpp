#include "transcriber_windows.h"

#include <QDebug>
#include <QtConcurrent/QtConcurrent>

#include <sapi.h>
#include <sphelper.h>
#include <windows.h>
#include <comdef.h>
#include <iostream>

#define SUBLANG_CHINESE_HONGKONG 0x03

void ListAvailableRecognizers() {
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to initialize COM. HRESULT: " << hr << std::endl;
        return;
    }

    ISpObjectTokenCategory* pCategory = nullptr;
    hr = SpGetCategoryFromId(SPCAT_RECOGNIZERS, &pCategory);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to get recognizer category. HRESULT: " << hr << std::endl;
        CoUninitialize();
        return;
    }

    IEnumSpObjectTokens* pEnum = nullptr;
    hr = pCategory->EnumTokens(NULL, NULL, &pEnum);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to enumerate recognizers. HRESULT: " << hr << std::endl;
        pCategory->Release();
        CoUninitialize();
        return;
    }

    ULONG count = 0;
    hr = pEnum->GetCount(&count);
    if (FAILED(hr) || count == 0) {
        std::wcerr << L"No recognizers found." << std::endl;
        pEnum->Release();
        pCategory->Release();
        CoUninitialize();
        return;
    }

    for (ULONG i = 0; i < count; ++i) {
        ISpObjectToken* pToken = nullptr;
        hr = pEnum->Next(1, &pToken, NULL);
        if (SUCCEEDED(hr) && pToken) {
            // Get the recognizer description
            WCHAR* pszDescription = nullptr;
            hr = SpGetDescription(pToken, &pszDescription);
            if (SUCCEEDED(hr)) {
                std::wcout << L"Recognizer: " << pszDescription << std::endl;
                CoTaskMemFree(pszDescription);
            }

            // Get the supported languages
            WCHAR* pszLanguage = nullptr;
            hr = pToken->GetStringValue(L"Language", &pszLanguage);
            if (SUCCEEDED(hr)) {
                std::wcout << L" - Supported Language(s): " << pszLanguage << std::endl;
                CoTaskMemFree(pszLanguage);
            }

            pToken->Release();
        }
    }

    // Clean up
    pEnum->Release();
    pCategory->Release();
    CoUninitialize();
}

#include <imm.h>

void getkeyboardLayouts(wchar_t *layoutName) {
    // Get the number of installed keyboard layouts
    int numLayouts = GetKeyboardLayoutList(0, nullptr);

    // Create a buffer to hold the layout identifiers
    HKL* layouts = new HKL[numLayouts];

    // Get the list of installed keyboard layouts
    GetKeyboardLayoutList(numLayouts, layouts);

    // Output the layout identifiers and their names
    for (int i = 0; i < numLayouts; i++) {
        // Get the name of the layout
        if (GetKeyboardLayoutName(layoutName)) {
            std::wcout << "Keyboard Layout " << i + 1 << ": " << layoutName << std::endl;
        } else {
            std::cout << "Error retrieving layout name." << std::endl;
        }
    }

    // Clean up
    delete[] layouts;
}


void EnumKeyboardLayoutsFromRegistry() {
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                               L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts",
                               0, KEY_READ, &hKey);

    if (result != ERROR_SUCCESS) {
        std::cout << "Failed to open registry key." << std::endl;
        return;
    }

    DWORD index = 0;
    WCHAR layoutKeyName[256];
    DWORD layoutKeyNameSize = sizeof(layoutKeyName) / sizeof(layoutKeyName[0]);

    while (RegEnumKeyEx(hKey, index, layoutKeyName, &layoutKeyNameSize,
                        NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {

        // Open the individual layout key
        HKEY hLayoutKey;
        std::wstring layoutPath = L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\";
        layoutPath += layoutKeyName;

        result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, layoutPath.c_str(), 0, KEY_READ, &hLayoutKey);

        if (result == ERROR_SUCCESS) {
            // Query for the Locale value
            DWORD locale;
            DWORD localeSize = sizeof(locale);
            result = RegQueryValueEx(hLayoutKey, L"Locale", NULL, NULL, (LPBYTE)&locale, &localeSize);

            if (result == ERROR_SUCCESS) {
                std::wcout << L"Layout ID: " << layoutKeyName << L", Locale: 0x" << std::hex << locale << std::dec << std::endl;
            } else {
                std::wcout << L"Layout ID: " << layoutKeyName << L" has no Locale value." << std::endl;
            }

            RegCloseKey(hLayoutKey);  // Close the layout key
        } else {
            std::wcout << L"Failed to open key for layout: " << layoutKeyName << std::endl;
        }

        layoutKeyNameSize = sizeof(layoutKeyName) / sizeof(layoutKeyName[0]);
        index++;
    }

    RegCloseKey(hKey);  // Close the main registry key
}

std::string wcharArrayToString(const wchar_t* wideArray) {
    return std::string(wideArray, wideArray + wcslen(wideArray));
}

Transcriber::Transcriber(std::string &locale)
{
    wchar_t layoutName[15];
    getkeyboardLayouts(layoutName);
    // Enumerate all installed keyboard layouts for the current thread
    EnumKeyboardLayoutsFromRegistry();

    std::string result = wcharArrayToString(layoutName);
    std::cout << result << std::endl;

    originalLayout = GetKeyboardLayout(0);

    if (result != "00000C04") {
        // HKL layout = LoadKeyboardLayout(L"00000804", KLF_ACTIVATE);
        HKL layout = LoadKeyboardLayout(L"00000C04", KLF_ACTIVATE);

        if (layout == NULL) {
            std::cerr << "Failed to load keyboard layout" << std::endl;
        }

        // Optionally, you can activate the layout using ActivateKeyboardLayout
        if (!ActivateKeyboardLayout(layout, KLF_ACTIVATE)) {
            std::cerr << "Failed to activate keyboard layout" << std::endl;
        }

        std::cout << "Keyboard layout switched successfully!" << std::endl;

        getkeyboardLayouts(layoutName);
        result = wcharArrayToString(layoutName);

        std::cout << result << std::endl;
        std::cout << (result == "00000C04") << std::endl;
    }

    // Simulate the Windows key down
    INPUT input[2] = {};

    // Windows key down event
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = VK_LWIN; // Left Windows key
    input[0].ki.dwFlags = 0;   // Key press

    // 'H' key down event
    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wVk = 'H';     // 'H' key
    input[1].ki.dwFlags = 0;   // Key press

    // Send the input events (Windows key + H key press)
    SendInput(2, input, sizeof(INPUT));

    // Now simulate key releases (Windows key + H key release)
    input[0].ki.dwFlags = KEYEVENTF_KEYUP;  // Key release
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;  // Key release

    // Send the input events (Windows key + H key release)
    SendInput(2, input, sizeof(INPUT));

    // Initialize COM
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

    // Create a voice object
    HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&voice);
    if (FAILED(hr)) {
        std::cout << "Failed to create speech voice instance." << std::endl;
        CoUninitialize();
        return;
    }

    // Create the recognizer object
    hr = CoCreateInstance(CLSID_SpInprocRecognizer, NULL, CLSCTX_INPROC_SERVER, IID_ISpRecognizer, (void **)&recognizer);
    if (FAILED(hr)) {
        std::cout << "Failed to create recognizer instance." << std::endl;
        voice->Release();
        CoUninitialize();
        return;
    }

    ISpObjectTokenCategory* pCategory = nullptr;
    hr = SpGetCategoryFromId(SPCAT_RECOGNIZERS, &pCategory);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to get recognizer category. HRESULT: " << hr << std::endl;
        recognizer->Release();
        CoUninitialize();
        return;
    }

    // Enumerate tokens
    IEnumSpObjectTokens* pEnum = nullptr;
    hr = pCategory->EnumTokens(NULL, NULL, &pEnum);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to enumerate recognizers. HRESULT: " << hr << std::endl;
        pCategory->Release();
        recognizer->Release();
        CoUninitialize();
        return;
    }

    ISpObjectToken* pToken = nullptr;
    ULONG count = 0;
    hr = pEnum->GetCount(&count);
    if (FAILED(hr) || count == 0) {
        std::wcerr << L"No recognizers found." << std::endl;
        pEnum->Release();
        pCategory->Release();
        recognizer->Release();
        CoUninitialize();
        return;
    }

    std::cout << count << std::endl;

    for (ULONG i = 0; i < count; ++i) {
        hr = pEnum->Next(1, &pToken, NULL);
        if (SUCCEEDED(hr) && pToken) {
            // Check if this recognizer supports the desired language
            WCHAR* pszLanguage = nullptr;
            hr = pToken->GetStringValue(L"SupportedLanguages", &pszLanguage);
            if (SUCCEEDED(hr)) {
                // Check if the language matches
                std::wstring langs(pszLanguage);
                std::wcout << "lang: " << langs << std::endl;
                CoTaskMemFree(pszLanguage);

                // Language IDs are semicolon-separated, like "409;C0A"
                size_t pos = langs.find(std::to_wstring(404));
                if (pos != std::wstring::npos) {
                    // Set this recognizer
                    hr = recognizer->SetRecognizer(pToken);
                    if (SUCCEEDED(hr)) {
                        std::wcout << L"Recognizer set for language: " << std::hex << 404 << std::endl;
                        pToken->Release();
                        break;
                    }
                }
            } else {
                std::cout << "couldn't get string value" << std::endl;
            }
            pToken->Release();
        }
    }

    // Create the audio input object (microphone)
    hr = CoCreateInstance(CLSID_SpMMAudioIn, NULL, CLSCTX_INPROC_SERVER, IID_ISpAudio, (void **)&audio);
    if (FAILED(hr)) {
        std::cout << "Failed to create ISpAudio instance." << std::endl;
        audio->Release();
        CoUninitialize();
        return;
    }

    // hr = audio->SetState(SPAS_STOP, SPFEI_ALL_EVENTS);
    // if (FAILED(hr)) {
    //     std::cout << "Failed to configure ISpAudio state: " << hr << std::endl;
    //     audio->Release();
    //     recognizer->Release();
    //     CoUninitialize();
    //     return;
    // }

    // Set the audio input for the recognizer
    hr = recognizer->SetInput(audio, TRUE); // TRUE means we're taking input from the microphone
    if (FAILED(hr)) {
        std::cout << "Failed to set audio input for recognizer." << std::endl;
        audio->Release();
        recognizer->Release();
        voice->Release();
        CoUninitialize();
        return;
    }

    // Create a grammar object for the recognition engine
    hr = recognizer->CreateRecoContext(&recoContext);
    if (FAILED(hr)) {
        std::cout << "Failed to create recognition context." << std::endl;
        audio->Release();
        recognizer->Release();
        voice->Release();
        CoUninitialize();
        return;
    }

    // Load a default grammar for continuous speech recognition
    hr = recoContext->CreateGrammar(0, &grammar);
    if (FAILED(hr)) {
        std::cout << "Failed to create grammar." << std::endl;
        recoContext->Release();
        audio->Release();
        recognizer->Release();
        voice->Release();
        CoUninitialize();
        return;
    }

    ListAvailableRecognizers();

    hr = grammar->LoadDictation(NULL, SPLO_STATIC);
    if (FAILED(hr)) {
        std::cout << "Failed to load dictation grammar." << hr << std::endl;
        grammar->Release();
        recoContext->Release();
        audio->Release();
        recognizer->Release();
        voice->Release();
        CoUninitialize();
        return;
    }
}

Transcriber::~Transcriber()
{
    CoUninitialize();
}

void Transcriber::subscribe(IInputVolumeSubscriber *subscriber)
{
    _volumeSubscribers.emplace(subscriber);
}

void Transcriber::unsubscribe(IInputVolumeSubscriber *subscriber)
{
    _volumeSubscribers.extract(subscriber);
}

void Transcriber::notifyVolumeResult(std::variant<std::system_error, float> result)
{
    for (const auto s : _volumeSubscribers) {
        s->volumeResult(result);
    }
}

void Transcriber::volumeResult(std::variant<std::system_error, float> result)
{
    notifyVolumeResult(result);
}

void Transcriber::subscribe(ITranscriptionResultSubscriber *subscriber)
{
    _transcriptionSubscribers.emplace(subscriber);
}

void Transcriber::unsubscribe(ITranscriptionResultSubscriber *subscriber)
{
    _transcriptionSubscribers.extract(subscriber);
}

void Transcriber::notifyTranscriptionResult(
    std::variant<std::system_error, std::string> result)
{
    for (const auto s : _transcriptionSubscribers) {
        s->transcriptionResult(result);
    }
}

void Transcriber::transcriptionResult(
    std::variant<std::system_error, std::string> transcription)
{
    notifyTranscriptionResult(transcription);
}

void Transcriber::startRecognition()
{
    // HRESULT hr = grammar->SetRuleState(NULL, NULL, SPRS_ACTIVE);
    // if (FAILED(hr)) {
    //     std::cout << "Failed to activate grammar rule." << hr <<  std::endl;
    //     grammar->Release();
    //     recoContext->Release();
    //     audio->Release();
    //     recognizer->Release();
    //     CoUninitialize();
    //     return;
    // }

    HRESULT hr = recognizer->SetRecoState(SPRST_ACTIVE); // Activate recognizer
    if (FAILED(hr)) {
        std::wcerr << L"Failed to activate recognizer. HRESULT: " << hr << std::endl;
        return;
    }

    hr = grammar->SetDictationState(SPRS_ACTIVE);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to activate dictation grammar. HRESULT: " << hr << std::endl;
        CoUninitialize();
        return;
    }

    // Create an event handle for recognition events
    HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!hEvent) {
        std::wcerr << L"Failed to create event handle. Error: " << GetLastError() << std::endl;
        return;
    }

    hr = recoContext->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION));
    if (FAILED(hr)) {
        std::wcerr << L"Failed to set interest for recognition events. HRESULT: " << hr << std::endl;
        return;
    }

    // Wait for recognition events (e.g., speech input)
    std::cout << "waiting for speech input..." << std::endl;

    // std::ignore = QtConcurrent::run([&]() {while (true) {

    //         std::cout << "waiting for speech input..." << std::endl;

    // SPEVENT event;
    // ULONG eventCount = 0;
    // HRESULT hr = recoContext->GetEvents(1, &event, &eventCount);  // Retrieve a single event

    // if (SUCCEEDED(hr) && eventCount > 0) {
    //     std::cout << "speech input!" << std::endl;
    //     // Process the event
    //     if (event.eEventId == SPEI_RECOGNITION) {
    //         std::cout << "speech recognition event" << std::endl;
    //         // Get the recognition result from the event structure
    //         ISpRecoResult *pResult = (ISpRecoResult *)event.lParam;  // The recognition result is stored in lParam

    //         if (pResult) {
    //             // Get the recognized text (transcription)
    //             WCHAR *recognizedText = NULL;
    //             hr = pResult->GetText(0, -1, TRUE, &recognizedText, NULL);
    //             if (SUCCEEDED(hr) && recognizedText != NULL) {
    //                 std::wcout << L"Recognized: " << recognizedText << std::endl;

    //                 size_t len = wcslen(recognizedText) + 1;  // +1 for the null terminator
    //                 char* charStr = new char[len];

    //                 // Convert the wide-character string to a multibyte string
    //                 wcstombs(charStr, recognizedText, len);

    //                 // Create an std::string from the converted multibyte string
    //                 std::string str(charStr);

    //                 // Output the result
    //                 notifyTranscriptionResult(str);

    //                 // Clean up dynamically allocated memory
    //                 delete[] charStr;

    //                 CoTaskMemFree(recognizedText);  // Don't forget to free the recognized text
    //             }
    //             pResult->Release();
    //         } else {
    //             std::cout << "pResult is empty" << std::endl;
    //         }
    //     }
    // }

    // // Allow other events to occur or manage other tasks
    // Sleep(100);  // Add a small delay to avoid maxing out CPU usage
    // }});
}

void Transcriber::stopRecognition()
{
    ActivateKeyboardLayout(originalLayout, KLF_ACTIVATE);

    ::CoUninitialize();
}
