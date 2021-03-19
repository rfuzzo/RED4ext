#include "stdafx.hpp"
#include "HookingManager.hpp"

void HookingManager::Create(std::shared_ptr<PluginBase> aPlugin, void* aTarget, void* aDetour, void** aOriginal)
{
    HookItem item{};
    item.plugin = aPlugin;
    item.target = aTarget;
    item.original = aOriginal;
    item.hook = std::make_unique<REDhook<>>(aTarget, aDetour);

    m_hooks.emplace(aPlugin, std::move(item));
}

void HookingManager::Remove(std::shared_ptr<PluginBase> aPlugin, void* aTarget)
{
    auto range = m_hooks.equal_range(aPlugin);
    for (auto it = range.first; it != range.second; ++it)
    {
        const auto& item = it->second;
        if (item.target == aTarget)
        {
            Detach(item);
            m_hooks.erase(it);

            break;
        }
    }
}

void HookingManager::RemoveAll()
{
    DetachAll();
    m_hooks.clear();
}

void HookingManager::RemoveAll(std::shared_ptr<PluginBase> aPlugin)
{
    DetachAll(aPlugin);
    m_hooks.erase(aPlugin);
}

bool HookingManager::Attach(std::shared_ptr<PluginBase> aPlugin, void* aTarget)
{
    auto range = m_hooks.equal_range(aPlugin);
    for (auto it = range.first; it != range.second; ++it)
    {
        const auto& item = it->second;
        if (item.target == aTarget)
        {
            return Attach(item);
        }
    }

    return false;
}

bool HookingManager::Detach(std::shared_ptr<PluginBase> aPlugin, void* aTarget)
{
    auto range = m_hooks.equal_range(aPlugin);
    for (auto it = range.first; it != range.second; ++it)
    {
        const auto& item = it->second;
        if (item.target == aTarget)
        {
            return Detach(item);
        }
    }

    return false;
}

void HookingManager::AttachAll() const
{
    for (const auto& [plugin, item] : m_hooks)
    {
        Attach(item);
    }
}

void HookingManager::DetachAll() const
{
    for (const auto& [plugin, item] : m_hooks)
    {
        Detach(item);
    }
}

void HookingManager::DetachAll(std::shared_ptr<PluginBase> aPlugin)
{
    auto range = m_hooks.equal_range(aPlugin);
    for (auto it = range.first; it != range.second; ++it)
    {
        const auto& item = it->second;
        Detach(item);
    }
}

bool HookingManager::Attach(const HookItem& aItem) const
{
    auto plugin = aItem.plugin;
    auto target = aItem.target;

    try
    {
        aItem.hook->attach();
        *aItem.original = aItem.hook->get_block_address();

        return aItem.hook->is_attached();
    }
    catch (const std::exception& ex)
    {
        spdlog::warn(L"An exception occured while attaching the hook at {:#x} for '{}'", target, plugin->GetName());
        spdlog::warn("{}", ex.what());
    }

    return false;
}

bool HookingManager::Detach(const HookItem& aItem) const
{
    auto plugin = aItem.plugin;
    auto target = aItem.target;

    try
    {
        *aItem.original = nullptr;
        aItem.hook->detach();

        return !aItem.hook->is_attached();
    }
    catch (const std::exception& ex)
    {
        spdlog::warn(L"An exception occured while detaching the hook at {:#x} for '{}'", target, plugin->GetName());
        spdlog::warn("{}", ex.what());
    }

    return false;
}
