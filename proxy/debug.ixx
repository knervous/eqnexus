#pragma once
export module debug;
import core;
import <thread>;
import <atomic>;
import <csignal>;
import <cstdlib>;
import <dinput.h>;
import <cstdio>;
import <iostream>;


bool ctrlRTriggered = false;
bool ctrlETriggered = false;
std::thread debugPollThread;
std::thread eventMessageThread;
std::atomic<bool> keepPolling{ false };

// Named event handles
HANDLE hUnloadEvent = 0;
HANDLE hReloadEvent = 0;

void PollKeyboard() {
    std::cout << "Starting keyboard polling loop." << std::endl;

    while (keepPolling) {
        if (!GetKeyboardDevice()) continue;

        BYTE keyboardState[256];
        HRESULT hr = GetKeyboardDevice()->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
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
            LoadDevices();
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

    hUnloadEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("Global\\UnloadEvent"));
    hReloadEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("Global\\ReloadEvent"));

    if (!hUnloadEvent || !hReloadEvent) {
        std::cerr << "Failed to create/open named events. Error: " << GetLastError() << std::endl;
        return;
    }

    while (keepPolling) {
        HANDLE events[] = { hUnloadEvent, hReloadEvent };
        DWORD waitResult = MsgWaitForMultipleObjects(2, events, FALSE, 0, QS_ALLINPUT);

        if (waitResult == WAIT_OBJECT_0) {
            std::cout << "Unload event triggered. Unloading module..." << std::endl;
            UnloadCore();
            ResetEvent(hUnloadEvent);
        }
        else if (waitResult == WAIT_OBJECT_0 + 1) {
            std::cout << "Reload event triggered. Reloading module..." << std::endl;
            LoadCore();
            LoadDevices();
            ResetEvent(hReloadEvent);
        }

        if (waitResult == WAIT_OBJECT_0 + 2) {
            MSG msg;
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    CloseHandle(hUnloadEvent);
    CloseHandle(hReloadEvent);
    hUnloadEvent = NULL;
    hReloadEvent = NULL;
}

export void StartDebugPoll() {
    if (!keepPolling.exchange(true)) {
        debugPollThread = std::thread(PollKeyboard);
        eventMessageThread = std::thread(PollEventsAndMessages);
    }
}

export void StopDebugPoll() {
    if (keepPolling.exchange(false)) {
        if (debugPollThread.joinable()) {
            debugPollThread.join();
        }
        if (eventMessageThread.joinable()) {
            eventMessageThread.join();
        }
    }
}

export void SignalHandler(int signal) {
    if (signal == SIGINT) {
        std::cout << "SIGINT received. Stopping debug poll..." << std::endl;
        StopDebugPoll();
    }
}