#include "stdafx.hpp"
#include "PluginBase.hpp"

PluginBase::PluginBase(RED4ext::PluginHandle aHandle)
    : m_handle(aHandle)
    , m_interface(nullptr)
{
    m_updateProc = GetProcAddress(aHandle, "Update");
}

void* PluginBase::GetUpdateProc() const
{
    return m_updateProc;
}

void PluginBase::SetInterface(void* aInterface)
{
    m_interface = aInterface;
}

void* PluginBase::GetInterface() const
{
    return m_interface;
}

RED4ext::PluginHandle PluginBase::GetHandle() const
{
    return m_handle;
}
