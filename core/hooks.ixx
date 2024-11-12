export module hooks;

import globals;
import <Windows.h>;
import <MinHook.h>;
import <thread>;
import <functional>;
import <tuple>;
import <functional>;
import <string_view>;
import <iostream>;
import <thread>;
import <chrono>;

using main_cb_t = std::function<void()>;

export class Hooks
{
   public:
    static void Init(main_cb_t cb)
    {
        eqlib::EQGameBaseAddress = (uintptr_t) GetModuleHandle(NULL);
        eqlib::InitializeGlobals();
        std::thread t([cb]() {
            while (!eqlib::EQMainBaseAddress || !eqlib::EQGraphicsBaseAddress)
            {
                eqlib::EQMainBaseAddress     = (uintptr_t) GetModuleHandle(L"eqmain.dll");
                eqlib::EQGraphicsBaseAddress = (uintptr_t) GetModuleHandle(L"EQGraphicsDX9.dll");
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
            cb();
        });
        t.detach();
    }

    static void Teardown()
    {
        eqlib::CleanupEQMainOffsets();
    }
};
