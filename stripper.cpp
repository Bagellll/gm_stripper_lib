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

// contains definitions for globals such as engine, gamedll clients, etc.
#include "plugin.hpp"

#include "stripper.h"
#include "icommandline.h"

#include <string>

#include <cstdio>
#include <cstddef>

static std::string g_mapname;
static stripper_core_t stripper_core;
static char game_path[256];
static char stripper_path[256];
static char stripper_cfg_path[256];

static void
log_message(const char* fmt, ...)
{
    va_list ap;
    char buffer[1024];

    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    buffer[sizeof(buffer) - 1] = '\0';

    g_SMAPI->LogMsg(g_PLAPI, "%s", buffer);
}

static void
path_format(char* buffer, size_t maxlength, const char* fmt, ...)
{
    va_list ap;
    char new_buffer[1024];

    va_start(ap, fmt);
    vsnprintf(new_buffer, sizeof(new_buffer), fmt, ap);
    va_end(ap);

    new_buffer[sizeof(new_buffer) - 1] = '\0';

    g_SMAPI->PathFormat(buffer, maxlength, "%s", new_buffer);
}

static const char*
get_map_name()
{
    return STRING(g_SMAPI->GetCGlobals()->mapname);
}

stripper_game_t stripper_game =
{
    NULL,
    NULL,
    NULL,
    log_message,
    path_format,
    get_map_name,
};

ConVar cvar_stripper_cfg_path("stripper_cfg_path", "addons/stripper", FCVAR_NONE, "Stripper Config Path");

ConVar stripper_curfile("stripper_current_file", "", FCVAR_SPONLY | FCVAR_NOTIFY, "Stripper for current map");

ConVar stripper_nextfile("stripper_next_file", "", FCVAR_PROTECTED | FCVAR_SPONLY, "Stripper for next map");

ConVar stripper_lowercase("stripper_file_lowercase", "0", FCVAR_NONE, "Load stripper configs in lowercase");

void stripper_cfg_path_changed(IConVar *var, const char *pOldValue, float flOldValue)
{
    strncpy(stripper_cfg_path, cvar_stripper_cfg_path.GetString(), sizeof(stripper_cfg_path));
}

bool
StripperPlugin::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
    PLUGIN_SAVEVARS();

    engine->GetGameDir(game_path, sizeof(game_path));
    stripper_game.game_path = game_path;
    stripper_game.stripper_path = "addons/stripper";
    stripper_game.stripper_cfg_path = stripper_cfg_path;
    strncpy(stripper_cfg_path, cvar_stripper_cfg_path.GetString(), sizeof(stripper_cfg_path));

    cvar_stripper_cfg_path.InstallChangeCallback( stripper_cfg_path_changed );

	const char* temp = (g_pCVar == NULL) ? NULL : g_pCVar->GetCommandLineValue("+stripper_path");

    if (temp != NULL && temp[0] != '\0')
    {
        g_SMAPI->PathFormat(stripper_path, sizeof(stripper_path), "%s", temp);
        stripper_game.stripper_path = stripper_path;
    }

    LoadStripper(&stripper_game, &stripper_core);

    return true;
}

bool
StripperPlugin::Unload(char *error, size_t maxlen)
{
    stripper_core.unload();

    return true;
}

const char*
GetMapEntitiesString_handler()
{
    return stripper_core.ent_string();
}

LevelInit_Data_t
LevelInit_handler(char const *pMapName, char const *pMapEntities, char const *pOldLevel, char const *pLandmarkName, bool loadGame, bool background)
{
    if (strlen(stripper_nextfile.GetString()) > 0) {
        g_mapname.assign(stripper_nextfile.GetString());
        log_message("Loading %s for map \"%s\"", g_mapname.c_str(), pMapName);
    } else if (stripper_lowercase.GetInt()) {
        char* name = UTIL_ToLowerCase(pMapName);
        g_mapname.assign(name);
        delete[] name;
    } else {
        g_mapname.assign(pMapName);
    }

    stripper_nextfile.SetValue("");
    stripper_curfile.SetValue(g_mapname.c_str());

    const char *ents = stripper_core.parse_map(g_mapname.c_str(), pMapEntities);

    return LevelInit_Data_t
    {
        pMapName,
        ents,
        pOldLevel,
        pLandmarkName,
        loadGame,
        background
    };
}

char*
UTIL_ToLowerCase(const char *str)
{
	size_t len = strlen(str);
	char *buffer = new char[len + 1];
	for (size_t i = 0; i < len; i++)
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
			buffer[i] = str[i] - ('A' - 'a');
		else
			buffer[i] = str[i];
	}
	buffer[len] = '\0';
	return buffer;
}

bool
StripperPlugin::Pause(char *error, size_t maxlen)
{
    return true;
}

bool
StripperPlugin::Unpause(char *error, size_t maxlen)
{
    return true;
}

void
StripperPlugin::AllPluginsLoaded()
{
}

const char*
StripperPlugin::GetAuthor()
{
    return "BAILOPAN";
}

const char*
StripperPlugin::GetName()
{
    return "Stripper";
}

const char*
StripperPlugin::GetDescription()
{
    return "Strips/Adds Map Entities";
}

const char*
StripperPlugin::GetURL()
{
    return "http://www.bailopan.net/";
}

const char*
StripperPlugin::GetLicense()
{
    return "GPL v3";
}

const char*
StripperPlugin::GetVersion()
{
    return STRIPPER_FULL_VERSION;
}

const char*
StripperPlugin::GetDate()
{
    return __DATE__;
}

const char*
StripperPlugin::GetLogTag()
{
    return "STRIPPER";
}

static int last_command_client = 1;

void
Stripper_SetCommandClient(int client)
{
    last_command_client = client;
}

ConVar stripper_version("stripper_version", STRIPPER_FULL_VERSION, FCVAR_SPONLY | FCVAR_NOTIFY, "Stripper Version");

CON_COMMAND(stripper_dump, "Dumps the map entity list to a file")
{
    if (last_command_client != -1)
        return;

    stripper_core.command_dump();
}

