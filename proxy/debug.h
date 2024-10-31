#pragma once
#ifdef _DEBUG
#include <thread>
#include <atomic>
#include <csignal>
#include <cstdlib>
#include <dinput.h>
#include <cstdio>
#include <iostream>
#include "core.h"

bool ctrlRTriggered = false;
bool ctrlETriggered = false;
std::thread debugPollThread;
std::thread eventMessageThread;
std::atomic<bool> keepPolling{ false };

// Named event handles
HANDLE hUnloadEvent = NULL;
HANDLE hReloadEvent = NULL;

void PollKeyboard() {
    std::cout << "Starting keyboard polling loop." << std::endl;

    while (keepPolling) {
        if (!pKeyboardDevice) continue;

        BYTE keyboardState[256];
        HRESULT hr = pKeyboardDevice->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
        if (FAILED(hr)) {
            return;
        }

        bool ctrlPressed = (keyboardState[DIK_LCONTROL] & 0x80) || (keyboardState[DIK_RCONTROL] & 0x80);
        bool rPressed = (keyboardState[DIK_R] & 0x80);
        bool ePressed = (keyboardState[DIK_E] & 0x80);

        if (ctrlPressed && rPressed && !ctrlRTriggered && !core_loaded) {
            std::cout << "Reloading module..." << std::endl;
            ctrlRTriggered = true;
            LoadCore();
        }
        if (ctrlPressed && ePressed && !ctrlETriggered && core_loaded) {
            std::cout << "Unloading module..." << std::endl;
            ctrlETriggered = true;
            UnloadCore();
        }

        if (!ctrlPressed || !rPressed) {
            ctrlRTriggered = false;
        }
        if (!ctrlPressed || !ePressed) {
            ctrlETriggered = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void PollEventsAndMessages() {
    std::cout << "Starting event and message polling loop." << std::endl;

    // Create or open the named events
    hUnloadEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("Global\\UnloadEvent"));
    hReloadEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("Global\\ReloadEvent"));

    if (!hUnloadEvent || !hReloadEvent) {
        std::cerr << "Failed to create/open named events. Error: " << GetLastError() << std::endl;
        return;
    }

    while (keepPolling) {
        // Check both unload and reload events at once
        HANDLE events[] = { hUnloadEvent, hReloadEvent };
        DWORD waitResult = MsgWaitForMultipleObjects(2, events, FALSE, 0, QS_ALLINPUT);

        if (waitResult == WAIT_OBJECT_0) {  // hUnloadEvent was signaled
            std::cout << "Unload event triggered. Unloading module..." << std::endl;
            UnloadCore();
            ResetEvent(hUnloadEvent);
        }
        else if (waitResult == WAIT_OBJECT_0 + 1) {  // hReloadEvent was signaled
            std::cout << "Reload event triggered. Reloading module..." << std::endl;
            LoadCore();
            ResetEvent(hReloadEvent);
        }

        // Process any pending input messages
        if (waitResult == WAIT_OBJECT_0 + 2) {
            MSG msg;
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Clean up event handles
    CloseHandle(hUnloadEvent);
    CloseHandle(hReloadEvent);
    hUnloadEvent = NULL;
    hReloadEvent = NULL;
}

void StartDebugPoll() {
    if (!keepPolling.exchange(true)) {
        debugPollThread = std::thread(PollKeyboard);
        eventMessageThread = std::thread(PollEventsAndMessages);
    }
}

void StopDebugPoll() {
    if (keepPolling.exchange(false)) {
        if (debugPollThread.joinable()) {
            debugPollThread.join();
        }
        if (eventMessageThread.joinable()) {
            eventMessageThread.join();
        }
    }
}

void SignalHandler(int signal) {
    if (signal == SIGINT) {
        std::cout << "SIGINT received. Stopping debug poll..." << std::endl;
        StopDebugPoll();
    }
}

#endif
