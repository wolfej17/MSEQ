
#include "MSEQPluginGUI.h"

MSEQPluginGUI::MSEQPluginGUI()
{
}

ADD_AUDIOPLUGIN_CLASS_TO_CONTAINER(
    MSEQ,            // Name of the plug-in container for this shared library
    MSEQPluginGUI,   // Authoring plug-in class to add to the plug-in container
    MSEQFX           // Corresponding Sound Engine plug-in class
);
