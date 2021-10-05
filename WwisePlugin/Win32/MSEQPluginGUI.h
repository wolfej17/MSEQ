#pragma once

#include "../MSEQPlugin.h"

class MSEQPluginGUI final
	: public AK::Wwise::Plugin::PluginMFCWindows<>
	, public AK::Wwise::Plugin::GUIWindows
{
public:
	MSEQPluginGUI();

};
