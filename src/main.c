#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../raylib/src/raylib.h"

#define _TO_STRING(name) #name
#define TO_STRING(name) _TO_STRING(name)

#ifndef DEBUG
#define DEBUG 0
#endif

#define TARGET_FPS 60
#define MAX_FONT_SIZE 48
#define MAX_NAME_LENGTH 256
#define MAX_BUFFER_SIZE 2048

#define WIDTH 900
#define HEIGHT 550

#define LOGO_WIDTH (HEIGHT * .3f)
#define LOGO_HEIGHT LOGO_WIDTH
#define LOGO_BAN_OFFSET 50.0f

#define SCROLLABLE_WIDTH (WIDTH * .60f)
#define SCROLLABLE_HEIGHT (HEIGHT * .80f)
#define SCROLLABLE_LINE_HEIGHT (SCROLLABLE_HEIGHT * .10f)
#define TEXT_FONT_SIZE 20
#define HEADER_MARGIN 20.0f
#define HEADER_FONT_SIZE 26
#define CHECKBOX_DIMENSIONS 20.0f
#define CHECKBOX_RESERVED_SPACE SCROLLABLE_LINE_HEIGHT

#define RES_PATH "./res/"
#define TEXTURE_PATH RES_PATH "img/"
#define FONT_PATH RES_PATH "fonts/"

#define ASP_FREE(ptr) free(ptr)
#define ASP_MALLOC(size, type) malloc((size) * sizeof(type))
#define ASP_CSTR_JOIN(a, b, buffer)                           \
    do {                                                      \
        size_t len_a = strlen(a);                             \
        size_t len_b = strlen(b);                             \
        memcpy((void *)(buffer), (void *)(a), len_a);         \
        memcpy((void *)&(buffer)[len_a], (void *)(b), len_b); \
        buffer[len_a + len_b] = '\0';                         \
    } while (0);
// Needs prior defintion of `result` variable.
// It is a simple implementation of a defer statement that to be used also needs a `defer` label at the end of the
// function implementation that does what you need and specifically `returns result`
#define RETURN_DEFER(value) \
    do {                    \
        result = (value);   \
        goto defer;         \
    } while (0)

#define LOG_TEMPLATE   \
    __TIME__           \
    " - [%s] "__FILE__ \
    ":" TO_STRING(__LINE__) ": "
#define ASP_ERROR(text, ...)                               \
    do {                                                   \
        char buffer[MAX_BUFFER_SIZE] = {0};                \
        snprintf(buffer, MAX_PATH, LOG_TEMPLATE, "ERROR"); \
                                                           \
        char format[MAX_BUFFER_SIZE] = {0};                \
        snprintf(format, MAX_PATH, text, __VA_ARGS__);     \
                                                           \
        char out[MAX_BUFFER_SIZE] = {0};                   \
        ASP_CSTR_JOIN(buffer, format, out);                \
        fprintf(stderr, out, __VA_ARGS__);                 \
        exit(EXIT_FAILURE);                                \
    } while (0)

#define TODO() assert(0 && "Not yet implemented")
#define Len(arr) sizeof(arr) / sizeof(arr[0])
#define RectUnpack(rect) rect.x, rect.y, rect.width, rect.height
#define V2Unpack(vec) vec.x, vec.y
#define V2Add(v, vv) \
    (Vector2) { v.x + vv.x, v.y + vv.y }

#define HEADLINE "HEAD#"
#define TEXTLINE "TEXT#"
#define TAG_LEN 5

typedef struct Cstr_Arr {
    char **data;
    size_t allocated;
    size_t capacity;
} Cstr_Arr;

// Reallocates entire structure and doubles the capacity
#define Cstr_Arr_Reallocate(arrPtr, mult)                               \
    do {                                                                \
        (&stringBuilder)->capacity *= mult;                             \
        Cstr_Arr new;                                                   \
        new.data = malloc((&stringBuilder)->capacity * sizeof(char *)); \
        for (size_t i = 0; i < (&stringBuilder)->allocated; i++) {      \
            new.data[i] = (&stringBuilder)->data[i];                    \
        }                                                               \
        free((void *)(&stringBuilder)->data);                           \
        new.capacity = (&stringBuilder)->capacity;                      \
        new.allocated = (&stringBuilder)->allocated;                    \
        *(&stringBuilder) = new;                                        \
    } while (0)

#define Cstr_Arr_AddElem(arrPtr, elem, mult)                \
    do {                                                    \
        if ((arrPtr)->allocated == (arrPtr)->capacity) {    \
            Cstr_Arr_Reallocate(arrPtr, mult);              \
        }                                                   \
        (arrPtr)->data[(arrPtr)->allocated] = (char *)elem; \
        (arrPtr)->allocated++;                              \
    } while (0)

#define Cstr_Arr_FillFromSplit(arrPtr, str, split, mult)                    \
    do {                                                                    \
        if ((arrPtr)->data == NULL) {                                       \
            (arrPtr)->data = malloc(1 * sizeof(char *));                    \
            (arrPtr)->capacity = 1;                                         \
        }                                                                   \
        size_t prev = 0;                                                    \
        for (size_t i = 0; i < strlen(str); i++) {                          \
            char *cur = malloc((i - prev) * sizeof(char) + 1);              \
            memcpy((void *)cur, (void *)&(str)[prev], i - prev);            \
            cur[i - prev] = '\0';                                           \
            if ((str)[i] == (split) && i - prev > 0) {                      \
                Cstr_Arr_AddElem((arrPtr), cur, (mult));                    \
                prev = i + 1;                                               \
                i++;                                                        \
                assert((str)[i + 1] != (split) && "Invalid string format"); \
            }                                                               \
        }                                                                   \
        if (prev == strlen(str)) break;                                     \
        Cstr_Arr_AddElem((arrPtr), &(str)[prev], (mult));                   \
    } while (0)

#define Cstr_Arr_ForEach(arrPtr, callback)                 \
    do {                                                   \
        for (size_t i = 0; i < (arrPtr)->allocated; i++) { \
            callback(arrPtr, i);                           \
        }                                                  \
    } while (0)

#define Cstr_Arr_Join(arrPtr, sep, out)                       \
    do {                                                      \
        char buffer[MAX_PATH] = {0};                          \
        size_t prev = 0;                                      \
        for (size_t i = 0; i < (arrPtr)->allocated; i++) {    \
            size_t curLen = strlen((arrPtr)->data[i]);        \
            if (curLen == 0) continue;                        \
            memcpy(&buffer[prev], (arrPtr)->data[i], curLen); \
            prev += curLen;                                   \
            memcpy(&buffer[prev], sep, strlen(sep));          \
            prev += strlen(sep);                              \
        }                                                     \
        buffer[prev - 1] = '\0';                              \
        memcpy(out, buffer, strlen(buffer));                  \
        out[strlen(buffer)] = '\0';                           \
    } while (0)

// Colors
#define DEBUG_COLOR \
    (Color) { 229, 0, 232, 255 }
#define BG_COLOR \
    (Color) { 33, 33, 33, 255 }

#define FONTS                        \
    FONT_PATH "RobotoSlab-Bold.ttf", \
        FONT_PATH "RobotoSlab-Regular.ttf"

typedef enum {
    ROBOTO_SLAB_BOLD,
    ROBOTO_SLAB_REGULAR,
    FONTS_LENGTH
} FontIndex;

#define TEXTURES                      \
    TEXTURE_PATH "icon.png",          \
        TEXTURE_PATH "logo.png",      \
        TEXTURE_PATH "cancel.png",    \
        TEXTURE_PATH "box-empty.png", \
        TEXTURE_PATH "box-filled.png"

typedef enum {
    ICON,
    LOGO,
    CANCEL,
    BOX_EMPTY,
    BOX_FILLED,
    TEXTURES_LENGTH
} TextureIndex;

static Font fonts[FONTS_LENGTH] = {0};
static Texture textures[TEXTURES_LENGTH] = {0};
static Vector2 scrollableAnchor;
static float scrollOffset = 0.0f;

long long WriteToDataFile(const char *, const char *);

#ifdef _WIN32

// Windows locations
// https://stackoverflow.com/questions/39953495/how-to-completely-uninstall-android-studio-from-windowsv10
// 1) Run default uninstaller
// 2) Remove studio files
//    - %USERPROFILE%/.android
//    - %USERPROFILE%/.gradle
//    - %USERPROFILE%/.m2
//    - %USERPROFILE%/.AndroidStudio* (analogues)
//    - %APPDATA%/JetBrains
//    - %APPDATA%/Google/AndroidStudio*
//    - %LOCALAPPDATA%/Google/AndroidStudio*
//    - C:/Program Files/Android
// 3) Remove SDK
//    - %LOCALAPPDATA%/Android
// 4) Delete Android Studio projects
//    - %USERPROFILE%/AndroidStudioProjects

#include <process.h>
#include <windows.h>

#define DATA_DIR_PATH "%APPDATA%/AndroidStudioPurge/"
#define DATA_FILE_PATH DATA_DIR_PATH ".boxes"

#define STUDIO_FILES_PATH                                \
    HEADLINE "Remove studio files:",                     \
        TEXTLINE "%USERPROFILE%/.android",               \
        TEXTLINE "%USERPROFILE%/.gradle",                \
        TEXTLINE "%USERPROFILE%/.m2",                    \
        TEXTLINE "%USERPROFILE%/.AndroidStudio*",        \
        TEXTLINE "%APPDATA%/JetBrains",                  \
        TEXTLINE "%APPDATA%/Google/AndroidStudio*",      \
        TEXTLINE "%LOCALAPPDATA%/Google/AndroidStudio*", \
        TEXTLINE "C:/Program Files/Android"
#define SDK_PATH            \
    HEADLINE "Remove SDK:", \
        TEXTLINE "%LOCALAPPDATA%/Android"
#define USER_PATH                             \
    HEADLINE "Delete user-created projects:", \
        TEXTLINE "%USERPROFILE%/AndroidStudioProjects"

#define REMOVE_PATHS   \
    STUDIO_FILES_PATH, \
        SDK_PATH,      \
        USER_PATH,

#define TOTAL_REMOVE_PATHS TOTAL_ENTRIES - TOTAL_HEADLINES

enum {
    _STUDIO_FILES_PATH,
    _SDK_PATH,
    _USER_PATH,
    TOTAL_HEADLINES
};

typedef enum {
    // STUDIO_FILES_PATH
    STUDIO_PATHS_HEADLINE,
    DOT_ANDROID,
    DOT_GRADLE,
    DOT_M2,
    DOT_ANDROID_STUDIO_STAR,
    JETBRAINS,
    ANDROID_STUDIO_STAR_APPDATA,
    ANDROID_STUDIO_STAR_LOCAL,
    ANDROID,
    // SDK_PATH
    SDK_PATH_HEADLINE,
    SDK_ANDROID_LOCAL,
    // USER_PATH
    USER_PATH_HEADLINE,
    USER_STUDIO_PROJECTS,
    // Total entries
    TOTAL_ENTRIES
} RemovePathsIndex;

bool checkboxes[TOTAL_ENTRIES] = {
    0,  // STUDIO_PATHS_HEADLINE
    1,  // DOT_ANDROID
    1,  // DOT_GRADLE
    1,  // DOT_M2
    1,  // DOT_ANDROID_STUDIO_STAR
    1,  // JETBRAINS
    1,  // ANDROID_STUDIO_STAR_APPDATA
    1,  // ANDROID_STUDIO_STAR_LOCAL
    1,  // ANDROID
    0,  // SDK_PATH_HEADLINE
    0,  // SDK_ANDROID_LOCAL
    0,  // USER_PATH_HEADLINE
    0,  // USER_STUDIO_PROJECTS
};

// Takes `Cstr_Arr*` and attempts to swap the `idx-th` element
// with the contents of the environment variable
void TranslateEnvVariable(Cstr_Arr *arrPtr, size_t idx) {
    int result = 0;

    const char *variable = (arrPtr)->data[(idx)];
    if (strlen(variable) < 2 || *variable != '%' || variable[strlen(variable) - 1] != '%') return;

    char name[MAX_NAME_LENGTH] = {0};
    memcpy(name, &variable[1], strlen(variable) - 2);

    char const *envVar = name;

    DWORD code = GetEnvironmentVariable(envVar, name, MAX_NAME_LENGTH);
    DWORD error = GetLastError();
    char *errorMsg;
    if (error == ERROR_ENVVAR_NOT_FOUND) {
        errorMsg = "Enviroment variable %s not found";
        RETURN_DEFER(1);
    }
    if (code == MAX_NAME_LENGTH) {
        errorMsg = "Enviroment variable %s exceeded buffer length";
        RETURN_DEFER(1);
    }

    ASP_FREE((arrPtr)->data[(idx)]);
    (arrPtr)->data[(idx)] = malloc(strlen(name) * sizeof(char));
    memcpy((arrPtr)->data[(idx)], name, strlen(name) + 1);

defer:
    if (result) ASP_ERROR(errorMsg, variable);
    SetLastError(NO_ERROR);
    return;
}

void ParsePath(const char *path, void *dest) {
    Cstr_Arr stringBuilder = {0};
    Cstr_Arr_FillFromSplit(&stringBuilder, path, '/', 2);

    // TODO: the second time this thing won't work
    Cstr_Arr_ForEach(&stringBuilder, TranslateEnvVariable);

    char out[MAX_PATH] = {0};
    Cstr_Arr_Join(&stringBuilder, "\\", out);

    memcpy(dest, (void *)out, strlen(out) + 1);
}

void CreateDataDir(const char *path) {
    char dest[MAX_PATH] = {0};
    ParsePath(path, dest);
    const char *parsedPath = dest;
    if (!CreateDirectory(parsedPath, NULL)) {
        DWORD error = GetLastError();
        switch (error) {
            case ERROR_ALREADY_EXISTS:
                break;
            case ERROR_PATH_NOT_FOUND:
                ASP_ERROR("Path %s not found", path);
                break;
            default:
                ASP_ERROR("Folder creation you don't know about", NULL);
                break;
        }
        SetErrorMode(NO_ERROR);
    }
}

void CreateDataFile(const char *file) {
    char dest[MAX_PATH] = {0};
    ParsePath(file, dest);
    const char *parsedFile = dest;
    if (FileExists(parsedFile)) return;

    HANDLE hnd = CreateFile(parsedFile,
                            GENERIC_WRITE,
                            0,
                            0,
                            CREATE_NEW,
                            FILE_ATTRIBUTE_NORMAL,
                            0);
    if (hnd == INVALID_HANDLE_VALUE) ASP_ERROR("Invalid handle created for data file at: %s", parsedFile);

    char buffer[TOTAL_ENTRIES + 1] = {0};
    for (size_t i = 0; i < TOTAL_ENTRIES; i++) {
        buffer[i] = (checkboxes[i] ? '1' : '0');
    }

    WriteToDataFile(file, buffer);
}

void RemoveDirQuiet(LPCTSTR dir) {
    SHFILEOPSTRUCT file_op = {
        NULL,
        FO_DELETE,
        dir,
        "",
        FOF_CONFIRMMOUSE,
        false,
        0,
        ""};
    SHFileOperation(&file_op);
}

int startRemoval() {
    // log_trace("Starting Windows removal");

    // // * Running default uninstaller
    // log_info("Starting default uninstaller...");

    // // TODO: Search for installation path and run default uninstaller

    // // * Manual removal of Studio files
    // log_info("Manually deleting Studio files...");

    // // * Removal of Android SDK
    // log_info("Deleting Android SDK...");

    // // * Removal of projects created with Studio (User-created)
    // log_info("Deleting user's project folder...");

    return EXIT_SUCCESS;
}
#endif  // _WIN32

#ifdef __linux__

// linux stuff
//
// These are default locations as of Android Studio 1.0.2 and it is possible to change them by editing ..../android-studio/bin/idea.properties.
//
//     Delete the android-studio folder;
//     Delete the sdk folder if it is in not inside the android-studio directory;
//     Delete ~/.AndroidStudio, which contains config and system;
//     Delete ~/.android;
//     Delete ~/.local/share/applications/jetbrains-android-studio.desktop, if you created a shortcut using Configure->Create Desktop Entry.
//
// Note: Add on to step#5 above - Sometimes the icon launcher can be in one of the following locations to:
//
// /usr/share/applications
// /usr/local/share/applications
//
// If your launcher file is in any of the first two directories, you will require root permissions to remove it.
//
// PPAs
// A deeper step of removing Android Studio completely from your Linux Distribution would also include the removal of PPAs related to Android Studio.
//
// In Ubuntu 16.04,
//
//     Go to Software and Updates > Other Software
//     Scroll down the list and select to find the ones related to Android-studio
//     Click remove and authenticate
//
// This helps stopping updates and the very annoying error message:
// The package has not been installed. I can't find the archive for it.

// #include <unistd.h> in order to fork
#include <dirent.h>
#include <sys/types.h>

int startRemoval() {
    // Attempt to delete with snap

    // https://stackoverflow.com/questions/5237482/how-do-i-execute-an-external-program-within-c-code-in-linux-with-arguments

    // Check both /opt/ and /usr/local/
    DIR *dir = opendir("/opt/android-studio");
    if (dir) {
        closedir(dir);
    } else if (ENOENT == errno) {
        dir = opendir("/usr/local/android-studio");
        if (dir) {
            closedir(dir);
        } else if (ENOENT == errno) {
            /* Directory does not exist. */
        } else {
            /* opendir() failed for some other reason. */
        }
    } else {
        /* opendir() failed for some other reason. */
    }
    assert(false && "Not implemented");
    return EXIT_FAILURE;
}
#endif  // __linux__

#ifdef __APPLE__

// Mac locations
/*
# Deletes the Android Studio application
# Note that this may be different depending on what you named the application as, or whether you downloaded the preview version
rm -Rf /Applications/Android\ Studio.app
# Delete All Android Studio related preferences
# The asterisk here should target all folders/files beginning with the string before it
rm -Rf ~/Library/Preferences/AndroidStudio*
rm -Rf ~/Library/Preferences/Google/AndroidStudio*
# Deletes the Android Studio's plist file
rm -Rf ~/Library/Preferences/com.google.android.*
# Deletes the Android Emulator's plist file
rm -Rf ~/Library/Preferences/com.android.*
# Deletes mainly plugins (or at least according to what mine (Edric) contains)
rm -Rf ~/Library/Application\ Support/AndroidStudio*
rm -Rf ~/Library/Application\ Support/Google/AndroidStudio*
# Deletes all logs that Android Studio outputs
rm -Rf ~/Library/Logs/AndroidStudio*
rm -Rf ~/Library/Logs/Google/AndroidStudio*
# Deletes Android Studio's caches
rm -Rf ~/Library/Caches/AndroidStudio*
rm -Rf ~/Library/Caches/Google/AndroidStudio*
# Deletes older versions of Android Studio
rm -Rf ~/.AndroidStudio*
*/

int startRemoval() {
    assert(false && "Not implemented");
    return EXIT_FAILURE;
}
#endif  // __APPLE__

Vector2 getRectAnchor(float maxWidth, float maxHeight, float width, float height, Vector2 parentAnchor) {
    Vector2 offset = {(maxWidth - width) / 2, (maxHeight - height) / 2};
    return (Vector2){offset.x + parentAnchor.x, offset.y + parentAnchor.y};
}

long long WriteToDataFile(const char *file, const char *text) {
    long long result = 0;

    char dest[MAX_PATH] = {0};
    ParsePath(file, dest);
    char *parsedFile = dest;

    FILE *handle;
    fopen_s(&handle, parsedFile, "w");

    if (handle == NULL) RETURN_DEFER(-1);

    fprintf(handle, "%s", text);

    result = (long long)strlen(text);

defer:
    fclose(handle);
    if (result < 0) {
        ASP_ERROR("Encountered an error with path %s: %s", parsedFile, strerror(errno));
    }
    return result;
}

void LoadDataFile(const char *file) {
    int result = 0;

    char dest[MAX_PATH] = {0};
    ParsePath(file, dest);
    char *parsedFile = dest;

    FILE *handle = fopen(parsedFile, "r");

    if (handle == NULL) RETURN_DEFER(1);

    char buffer[MAX_NAME_LENGTH] = {0};
    fscanf(handle, "%s", buffer);

    for (size_t i = 0; i < TOTAL_ENTRIES; i++) {
        checkboxes[i] = (buffer[i] == '1');
    }

defer:
    fclose(handle);
    if (result) ASP_ERROR("Invalid file path %s", parsedFile);
    return;
}

void LoadResources() {
    const char *fontsPath[] = {FONTS};
    for (size_t i = 0; i < FONTS_LENGTH; i++) {
        fonts[i] = LoadFontEx(fontsPath[i], 48, NULL, 0);
    }

    const char *texturesPath[] = {TEXTURES};
    for (size_t i = 0; i < TEXTURES_LENGTH; i++) {
        textures[i] = LoadTexture(texturesPath[i]);
    }

    Vector2 anchor = getRectAnchor(WIDTH, HEIGHT, SCROLLABLE_WIDTH, SCROLLABLE_HEIGHT, (Vector2){0, 0});
    anchor.x = WIDTH - SCROLLABLE_WIDTH - anchor.x / 4;
    scrollableAnchor = anchor;
}

void UnloadResources() {
    for (size_t i = 0; i < FONTS_LENGTH; i++) {
        UnloadFont(fonts[i]);
    }

    for (size_t i = 0; i < TEXTURES_LENGTH; i++) {
        UnloadTexture(textures[i]);
    }
}

void WriteAppTitle() {
    const float marginTop = 55.0f;
    const float marginLeft = 60.0f;
    const float lineHeight = 50.0f;
    const float spacing = 5.0f;

    const char *lines[] = {
        "ANDROID",
        "STUDIO",
        "PURGE"};
    const size_t totalLines = sizeof(lines) / sizeof(lines[0]);

    float longest = 0;
    float offsets[totalLines];
    memset(offsets, 0, totalLines);
    for (size_t i = 0; i < totalLines; i++) {
        float textPixelLength = MeasureTextEx(fonts[ROBOTO_SLAB_BOLD], lines[i], MAX_FONT_SIZE, spacing).x;
        longest = max(longest, textPixelLength);
        offsets[i] = textPixelLength;
    }

    for (size_t i = 0; i < totalLines; i++) {
        offsets[i] = (longest - offsets[i]) / 2;
        RL_DrawTextEx(fonts[ROBOTO_SLAB_BOLD], lines[i], (Vector2){marginLeft + offsets[i], marginTop + lineHeight * i}, MAX_FONT_SIZE, spacing, RAYWHITE);
    }
}

void DrawAppLogo() {
    Vector2 anchor = getRectAnchor(HEIGHT, HEIGHT, LOGO_WIDTH, LOGO_HEIGHT, (Vector2){0, 0});
    anchor.x = 75.0f;
    anchor.y *= 1.35f;

    // Vector2 logoCenter = {anchor.x + LOGO_WIDTH / 2, anchor.y + LOGO_HEIGHT / 2};

    // ? Draw Android Studio logo texture
    DrawTexturePro(
        textures[LOGO],
        (RL_Rectangle){0, 0, 965, 966},
        (RL_Rectangle){V2Unpack(anchor), LOGO_WIDTH, LOGO_HEIGHT},
        (Vector2){0, 0},
        0.0f,
        WHITE);

    NPatchInfo patch = {(RL_Rectangle){0, 0, 379, 379}, 0, 0, 0, 0, NPATCH_NINE_PATCH};
    RL_Rectangle dest = (RL_Rectangle){
        anchor.x - LOGO_BAN_OFFSET / 2,
        anchor.y - LOGO_BAN_OFFSET / 2,
        LOGO_WIDTH + LOGO_BAN_OFFSET,
        LOGO_HEIGHT + LOGO_BAN_OFFSET};
    DrawTextureNPatch(
        textures[CANCEL],
        patch,
        dest,
        (Vector2){0, 0},
        0.0f,
        (Color){255, 255, 255, 255});

#if DEBUG
    // ? Draw Gizmos
    DrawRectangleLines(
        V2Unpack(anchor),
        LOGO_WIDTH,
        LOGO_HEIGHT,
        DEBUG_COLOR);
    DrawRectangleLines(RectUnpack(dest), DEBUG_COLOR);
#endif
}

void DrawScrollableOpts() {
    // const float lines = floorf(SCROLLABLE_HEIGHT / SCROLLABLE_LINE_HEIGHT);
    const int spacing = 2;
    static const char *const textArr[] = {
        REMOVE_PATHS};

    for (size_t i = 0; i < TOTAL_ENTRIES; i++) {
        size_t len = strlen(textArr[i]) - TAG_LEN;

        char curTag[TAG_LEN + 1] = {0};
        char curText[MAX_PATH] = {0};

        memcpy(curTag, textArr[i], TAG_LEN);
        memcpy(curText, &(textArr[i])[TAG_LEN], len);

        Font curFont;
        size_t marginLeft;
        int curFontSize;
        bool isText;
        if (strcmp(curTag, HEADLINE) == 0) {
            curFont = fonts[ROBOTO_SLAB_BOLD];
            marginLeft = HEADER_MARGIN;
            curFontSize = HEADER_FONT_SIZE;
            isText = false;
        } else if (strcmp(curTag, TEXTLINE) == 0) {
            curFont = fonts[ROBOTO_SLAB_REGULAR];
            marginLeft = CHECKBOX_RESERVED_SPACE;
            curFontSize = TEXT_FONT_SIZE;
            isText = true;
        } else {
            assert(false && "Invalid TAG used in text entry");
        }

        Vector2 rowAnchor = {scrollableAnchor.x + marginLeft, scrollableAnchor.y + SCROLLABLE_LINE_HEIGHT * i - scrollOffset};

#if DEBUG
        const Vector2 debugRowAnchor = {scrollableAnchor.x, scrollableAnchor.y + SCROLLABLE_LINE_HEIGHT * i - scrollOffset};
        DrawRectangleLines(V2Unpack(debugRowAnchor), SCROLLABLE_WIDTH, SCROLLABLE_LINE_HEIGHT, DEBUG_COLOR);
#endif

        const float textHeight = MeasureTextEx(curFont, curText, curFontSize, spacing).y;
        Vector2 textAnchor = rowAnchor;
        textAnchor.y += (SCROLLABLE_LINE_HEIGHT - textHeight) / 2;

        RL_DrawTextEx(
            fonts[ROBOTO_SLAB_REGULAR],
            curText,
            textAnchor,
            curFontSize, spacing, RAYWHITE);

        if (isText) {
            Vector2 boxAnchor = getRectAnchor(CHECKBOX_RESERVED_SPACE, CHECKBOX_RESERVED_SPACE, CHECKBOX_DIMENSIONS, CHECKBOX_DIMENSIONS, (Vector2){0, 0});
            boxAnchor.x += scrollableAnchor.x;
            boxAnchor.y += rowAnchor.y;

#if DEBUG
            DrawRectangleLines(V2Unpack(debugRowAnchor), CHECKBOX_RESERVED_SPACE, CHECKBOX_RESERVED_SPACE, DEBUG_COLOR);
#endif

            const RL_Rectangle boxDestRect = {V2Unpack(boxAnchor), CHECKBOX_DIMENSIONS, CHECKBOX_DIMENSIONS};

            const Texture boxTexture = textures[(checkboxes[i] ? BOX_FILLED : BOX_EMPTY)];

            DrawTextureNPatch(boxTexture,
                              (NPatchInfo){(RL_Rectangle){0, 0, 152, 152}, 0, 0, 0, 0, NPATCH_NINE_PATCH},
                              boxDestRect,
                              (Vector2){0, 0},
                              0.0f,
                              WHITE);
        } else {
            DrawLineEx(
                (Vector2){textAnchor.x, textAnchor.y + textHeight},
                (Vector2){textAnchor.x + MeasureTextEx(curFont, curText, curFontSize, spacing).x, textAnchor.y + textHeight},
                2, RAYWHITE);
        }
    }

    // Draw top and bottom wrapping rectangles
    DrawRectangle(scrollableAnchor.x, 0, SCROLLABLE_WIDTH, scrollableAnchor.y, BG_COLOR);
    DrawRectangle(scrollableAnchor.x, scrollableAnchor.y + SCROLLABLE_HEIGHT, SCROLLABLE_WIDTH, scrollableAnchor.y, BG_COLOR);
}

#define ScrollableRectContained(x, y) CheckContained((Vector2){x, y}, (RL_Rectangle){scrollableAnchor.x, scrollableAnchor.y, SCROLLABLE_WIDTH, SCROLLABLE_HEIGHT})

bool CheckContained(Vector2 point, RL_Rectangle space) {
    return (point.x >= space.x && point.x <= space.x + space.width) &&
           (point.y >= space.y && point.y <= space.y + space.height);
}

size_t getClickedRow(float x, float y) {
    assert(ScrollableRectContained(x, y) && "Invalid call of getClickedRow");
    return (size_t)floor((y - scrollableAnchor.y) / SCROLLABLE_LINE_HEIGHT + scrollOffset / SCROLLABLE_LINE_HEIGHT);
}

bool isRowHeader(size_t idx) {
    return (idx == STUDIO_PATHS_HEADLINE) || (idx == SDK_PATH_HEADLINE) || (idx == USER_PATH_HEADLINE);
}

void HandleClick(float x, float y) {
    if (ScrollableRectContained(x, y)) {
        size_t clickedRow = getClickedRow(x, y);
        if (isRowHeader(clickedRow)) return;
        checkboxes[clickedRow] ^= 1;
    }
}

void HandleScroll(float x, float y, float scroll) {
    if (ScrollableRectContained(x, y)) {
        scrollOffset += (-scroll) * SCROLLABLE_LINE_HEIGHT;
        if (scrollOffset < 0) scrollOffset = 0;
        const int scrollableLines = SCROLLABLE_HEIGHT / SCROLLABLE_LINE_HEIGHT;
        if (scrollOffset / SCROLLABLE_LINE_HEIGHT + scrollableLines >= TOTAL_ENTRIES)
            scrollOffset = (TOTAL_ENTRIES - scrollableLines) * SCROLLABLE_LINE_HEIGHT;
    }
}

int main(void) {
    InitWindow(WIDTH, HEIGHT, "Android Studio Purge");
    SetTargetFPS(TARGET_FPS);

    // * Create directory enviroment
    CreateDataDir(DATA_DIR_PATH);
    CreateDataFile(DATA_FILE_PATH);

    LoadDataFile(DATA_FILE_PATH);

    LoadResources();

    Image icon = LoadImageFromTexture(textures[ICON]);
    SetWindowIcon(icon);
    UnloadImage(icon);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BG_COLOR);
        WriteAppTitle();
        DrawAppLogo();
        DrawScrollableOpts();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            HandleClick(V2Unpack(GetMousePosition()));
        }

        const float scroll = GetMouseWheelMoveV().y;
        if (scroll != 0) {
            HandleScroll(V2Unpack(GetMousePosition()), scroll);
        }

        EndDrawing();
    }

    UnloadResources();
    RL_CloseWindow();

    char buffer[MAX_NAME_LENGTH] = {0};
    for (size_t i = 0; i < TOTAL_ENTRIES; i++) {
        buffer[i] = (checkboxes[i] ? '1' : '0');
    }
    WriteToDataFile(DATA_FILE_PATH, buffer);

    return 0;
}