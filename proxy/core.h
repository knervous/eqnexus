#pragma once
#include <dinput.h>

extern bool core_loaded;
extern IDirectInputDevice8A* pKeyboardDevice;
extern IDirectInputDevice8A* pMouseDevice;
void LoadCore();
void UnloadCore();