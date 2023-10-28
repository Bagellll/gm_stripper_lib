/** vim: set ts=4 sw=4 et tw=99:
 * 
 * === Stripper for Metamod:Source ===
 * Copyright (C) 2005-2009 David "BAILOPAN" Anderson
 * No warranties of any kind.
 * Based on the original concept of Stripper2 by botman
 *
 * License: see LICENSE.TXT
 * ===================================
 */

#pragma once

#ifndef _INCLUDE_SAMPLEPLUGIN_H
#define _INCLUDE_SAMPLEPLUGIN_H

#include "metamod/defs.h"
#include "metamod/sh_string.h"
#include "metamod/sh_list.h"
#include "metamod/sh_stack.h"

class StripperPlugin : public ISmmPlugin
{
public:
    bool Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late);
    bool Unload(char *error, size_t maxlen);
    bool Pause(char *error, size_t maxlen);
    bool Unpause(char *error, size_t maxlen);
    void AllPluginsLoaded();
public:
    const char *GetAuthor();
    const char *GetName();
    const char *GetDescription();
    const char *GetURL();
    const char *GetLicense();
    const char *GetVersion();
    const char *GetDate();
    const char *GetLogTag();
};

PLUGIN_GLOBALVARS();

struct LevelInit_Data_t
{
    const char* pMapName;
    const char* pMapEntities;
    const char* pOldLevel;
    const char* pLandmarkName;
    bool loadGame;
    bool background;
};

// parent project will call into these functions for results.
const char *GetMapEntitiesString_handler();
LevelInit_Data_t LevelInit_handler(char const *pMapName, char const *pMapEntities, char const *pOldLevel, char const *pLandmarkName, bool loadGame, bool background);
void Stripper_SetCommandClient(int client);
// ----------

char *UTIL_ToLowerCase(const char *str);

inline StripperPlugin g_Plugin;

#endif //_INCLUDE_SAMPLEPLUGIN_H
