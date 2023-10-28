#pragma once

#include "dbg.h"
#include "edict.h"

#include <cstddef>
#include <cstdio>

// Original forward definitions
extern CGlobalVars* gpGlobals;

class ISmmPlugin;
class ConCommandBase;

struct PluginId {}; // unused in practice

// All contents of this file below, in one way or another, belong to https://github.com/alliedmodders/metamod-source
// Modifications will be marked if said snippet is modified.
// Subject to the following license:

// The software is Copyright (C) 2004-2008, Metamod:Source Development Team.

// Metamod:Source is distributed under the "zLib/libpng" license, which is reproduced
// below:

// -----------------------------------------------------------------------------

// This software is provided "as-is", without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.

// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:

// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software in
//    a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.

// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.

// 3. This notice may not be removed or altered from any source distribution.

// -----------------------------------------------------------------------------

// The zLib/libpng license has been approved by the "Open Source Initiative"
// organization.

// -----------------------------------------------------------------------------

// From /loader/metamod_oslink.h
#if defined __WIN32__ || defined _WIN32 || defined WIN32
	#define WIN32_LEAN_AND_MEAN
	#define OS_WIN32
	#if defined _MSC_VER && _MSC_VER >= 1400
		#undef ARRAYSIZE
	#else
		#define mkdir(a) _mkdir(a)
	#endif
	#include <windows.h>
	#include <io.h>
	#include <direct.h>
	#define		dlmount(x)		LoadLibrary(x)
	#define		dlsym(x, s)		GetProcAddress(x, s)
	#define		dlclose(x)		FreeLibrary(x)
	const char*	dlerror();
	#define		abspath(x, s)	_fullpath(x, s, sizeof(x))
	#define	PATH_SEP_STR		"\\"
	#define PATH_SEP_CHAR		'\\'
	#define ALT_SEP_CHAR		'/'
	#define PATH_SIZE			MAX_PATH
	#define strcasecmp			stricmp
	inline bool _IsPathSepChar(char c) { return (c == '/' || c == '\\'); }
#elif defined __linux__ || defined __APPLE__
	#if defined __linux__
		#define OS_LINUX
	#elif defined __APPLE__
		#define OS_DARWIN
		#include <sys/syslimits.h>
	#endif
	#include <dlfcn.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <dirent.h>
	typedef		void*			HINSTANCE;
	#define		dlmount(x)		dlopen(x,RTLD_NOW)
	#define		abspath(x, s)	realpath(s, x)
	#define	PATH_SEP_STR		"/"
	#define PATH_SEP_CHAR		'/'
	#define ALT_SEP_CHAR		'\\'
	#define PATH_SIZE			PATH_MAX
	#ifndef stricmp
		#define	stricmp			strcasecmp
	#endif
	#ifndef strnicmp
		#define strnicmp		strncasecmp
	#endif
	inline bool _IsPathSepChar(char c) { return (c == '/'); }
#endif

// Added inline
inline size_t UTIL_FormatArgs(char *buffer, size_t maxlength, const char *fmt, va_list params)
{
	size_t len = vsnprintf(buffer, maxlength, fmt, params);

	if (len >= maxlength)
	{
		len = maxlength - 1;
		buffer[len] = '\0';
	}

	return len;
}

// Modified snippet from /core/ISmmAPI.h
class ISmmAPI
{
public:
	virtual void LogMsg(ISmmPlugin* pl, const char* msg, ...) = 0;

	virtual CGlobalVars* GetCGlobals() = 0;

	virtual size_t PathFormat(char* buffer, size_t len, const char* fmt, ...) = 0;
};

// Modified snippet from /core/ISmmPlugin.h
class ISmmPlugin
{
public:
	virtual const char *GetLogTag() = 0;
};

// Modified snippet from /core/metamod.cpp and /core/metamod.h
class MetamodSource : public ISmmAPI
{
public:
	// Made to work with this project
	void LogMsg(ISmmPlugin* pl, const char* msg, ...) override
	{
		va_list ap;
		char buffer[2048];
		
		va_start(ap, msg);
		UTIL_FormatArgs(buffer, sizeof(buffer), msg, ap);
		va_end(ap);

		Msg("[%s] %s\n", pl->GetLogTag(), buffer);
	}

	// Made to work with this project
	CGlobalVars* GetCGlobals() override
	{
		return gpGlobals;
	}

	size_t PathFormat(char* buffer, size_t len, const char* fmt, ...) override
	{
		va_list ap;
		va_start(ap, fmt);
		size_t mylen = UTIL_FormatArgs(buffer, len, fmt, ap);
		va_end(ap);

		for (size_t i = 0; i < mylen; i++)
		{
			if (buffer[i] == ALT_SEP_CHAR)
			{
				buffer[i] = PATH_SEP_CHAR;
			}
		}

		return mylen;
	}
};

// Modified to remove references to unused code from original source.
#define PLUGIN_SAVEVARS() \
	g_SMAPI = ismm; \
	g_PLAPI = static_cast<ISmmPlugin *>(this); \
	g_PLID = id;

// Modified to remove references to unused code from original source.
#define PLUGIN_GLOBALVARS()	\
	extern ISmmAPI *g_SMAPI; \
	extern ISmmPlugin *g_PLAPI; \
	extern PluginId g_PLID; 
