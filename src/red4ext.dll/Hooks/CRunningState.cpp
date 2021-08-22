#include "stdafx.hpp"
#include "Hooks/CRunningState.hpp"
#include "App.hpp"
#include "REDhook.hpp"

#include <RED4ext/GameApplication.hpp>

namespace
{
bool _CRunningState_Run(RED4ext::CRunningState* aThis, RED4ext::CGameApplication* aApp);
REDhook<decltype(&_CRunningState_Run)> CRunningState_Run;

bool _CRunningState_Run(RED4ext::CRunningState* aThis, RED4ext::CGameApplication* aApp)
{
    auto result = CRunningState_Run(aThis, aApp);

    auto app = App::Get();
    auto pluginsManager = app->GetPluginsManager();
    pluginsManager->Update();

    return result;
}
} // namespace

void CRunningState::Attach()
{
    new (&CRunningState_Run) REDhook<decltype(&_CRunningState_Run)>(
        {0x40, 0x53, 0x48, 0x83, 0xEC, 0x20, 0x48, 0x8B, 0x0D, 0xCC, 0xCC, 0xCC, 0xCC, 0x48, 0x8B, 0xDA},
        &_CRunningState_Run, 5, 3);

    CRunningState_Run.attach();
}

void CRunningState::Detach()
{
    CRunningState_Run.detach();
}
