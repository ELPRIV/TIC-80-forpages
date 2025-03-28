#ifndef SOKOL_SYS_INCLUDED
/*
    sokol_sys.h -- cross-platform system folder

    Project URL: https://github.com/floooh/sokol

    Do this:
        #define SOKOL_IMPL
    before you include this file in *one* C or C++ file to create the
    implementation.
*/

#define SOKOL_SYS_INCLUDED (1)

#ifndef SOKOL_API_DECL
#if defined(_WIN32) && defined(SOKOL_DLL) && defined(SOKOL_IMPL)
#define SOKOL_API_DECL __declspec(dllexport)
#elif defined(_WIN32) && defined(SOKOL_DLL)
#define SOKOL_API_DECL __declspec(dllimport)
#else
#define SOKOL_API_DECL extern
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

SOKOL_API_DECL const char* ssys_app_folder(const char *org, const char *app);

#ifdef __cplusplus
} /* extern "C" */

#endif
#endif // SOKOL_SYS_INCLUDED

#ifdef SOKOL_IMPL
#define SOKOL_SYS_IMPL_INCLUDED (1)

#ifndef SOKOL_API_IMPL
    #define SOKOL_API_IMPL
#endif
#ifndef SOKOL_DEBUG
    #ifndef NDEBUG
        #define SOKOL_DEBUG (1)
    #endif
#endif

#ifndef _SOKOL_PRIVATE
    #if defined(__GNUC__) || defined(__clang__)
        #define _SOKOL_PRIVATE __attribute__((unused)) static
    #else
        #define _SOKOL_PRIVATE static
    #endif
#endif

#if defined(_SAPP_APPLE)

#include <sys/stat.h>

SOKOL_API_DECL const char* ssys_app_folder(const char *org, const char *app)
{
    @autoreleasepool {
        static char result[1024];
        NSArray *array;

        if (!app) 
        {
            return NULL;
        }

        if (!org) 
        {
            org = "";
        }

        array = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);

        if ([array count] > 0) 
        { // we only want the first item in the list.
            NSString *str = [array objectAtIndex:0];
            const char *base = [str fileSystemRepresentation];
            if (base) 
            {

                char *ptr;
                if (*org) 
                {
                    snprintf(result, sizeof result, "%s/%s/%s/", base, org, app);
                }
                else
                {
                    snprintf(result, sizeof result, "%s/%s/", base, app);
                }

                for (ptr = result + 1; *ptr; ptr++) 
                {
                    if (*ptr == '/') 
                    {
                        *ptr = '\0';
                        mkdir(result, 0700);
                        *ptr = '/';
                    }
                }
                mkdir(result, 0700);
            }
        }

        return result;
    }
}

#elif defined(_SAPP_WIN32)

SOKOL_API_DECL const char* ssys_app_folder(const char *org, const char *app)
{
    //SHGetKnownFolderPath(SDL_FOLDERID_RoamingAppData, KF_FLAG_CREATE, NULL, &wszPath);

    return "";
}

#elif defined(_SAPP_LINUX)

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

SOKOL_API_DECL const char* ssys_app_folder(const char *org, const char *app)
{
    static char result[1024];

    const char *envr = getenv("XDG_DATA_HOME");
    const char *append;
    
    char *ptr = NULL;

    if (!app) 
    {
        return NULL;
    }

    if (!org) 
    {
        org = "";
    }

    if (!envr) 
    {
        envr = getenv("HOME");
        if (!envr) 
        {
            return NULL;
        }

        append = "/.local/share/";
    }
    else 
    {
        append = "/";
    }

    if (*org) 
    {
        snprintf(result, sizeof result, "%s%s%s/%s/", envr, append, org, app);
    } 
    else 
    {
        snprintf(result, sizeof result, "%s%s%s/", envr, append, app);
    }

    for (ptr = result + 1; *ptr; ptr++) 
    {
        if (*ptr == '/') 
        {
            *ptr = '\0';
            if (mkdir(result, 0700) != 0 && errno != EEXIST) 
            {
                goto error;
            }
            *ptr = '/';
        }
    }
    if (mkdir(result, 0700) != 0 && errno != EEXIST) 
    {
    error:
        return NULL;
    }

    return result;
}

#elif defined(_SAPP_EMSCRIPTEN)

SOKOL_API_DECL const char* ssys_app_folder(const char *org, const char *app)
{
    static char result[1024];

    snprintf(result, sizeof result, "/%s/%s/", org, app);

    return result;
}

#else

SOKOL_API_DECL const char* ssys_app_folder(const char *org, const char *app)
{
    return "";
}

#endif

#endif /* SOKOL_IMPL */
