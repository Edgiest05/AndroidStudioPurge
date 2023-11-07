#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../raylib/src/raylib.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#define TARGET_FPS 60

#define WIDTH 900
#define HEIGHT 550

#define LOGO_WIDTH (HEIGHT * .3f)
#define LOGO_HEIGHT LOGO_WIDTH
#define LOGO_BAN_OFFSET 50.0f

#define SCROLLABLE_WIDTH (WIDTH * .60f)
#define SCROLLABLE_HEIGHT (HEIGHT * .80f)
#define SCROLLABLE_LINE_HEIGHT (SCROLLABLE_HEIGHT * .10f)
#define CHECKBOX_DIMENSIONS 20.0f
#define CHECKBOX_RESERVED_SPACE SCROLLABLE_LINE_HEIGHT

#define RES_PATH "./res/"
#define TEXTURE_PATH RES_PATH "img/"
#define FONT_PATH RES_PATH "fonts/"

#define TODO() assert(0 && "Not yet implemented")
#define Len(arr) sizeof(arr) / sizeof(arr[0])
#define RectUnpack(rect) rect.x, rect.y, rect.width, rect.height
#define V2Unpack(vec) vec.x, vec.y
#define V2Add(v, vv) \
    (Vector2) { v.x + vv.x, v.y + vv.y }

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

// * Checkbox
typedef struct Checkbox {
    float width;
    float height;
    bool enabled;
} Checkbox;

void toggleCheckbox(Checkbox *box) {
    box->enabled = !box->enabled;
}

bool isCheckboxEnabled(Checkbox *box) {
    return box->enabled;
}

// * Button

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

#define STUDIO_FILES_PATH                       \
    "%USERPROFILE%/.android",                   \
        "%USERPROFILE%/.gradle",                \
        "%USERPROFILE%/.m2",                    \
        "%USERPROFILE%/.AndroidStudio*",        \
        "%APPDATA%/JetBrains",                  \
        "%APPDATA%/Google/AndroidStudio*",      \
        "%LOCALAPPDATA%/Google/AndroidStudio*", \
        "C:/Program Files/Android"

typedef enum {
    DOT_ANDROID,
    DOT_GRADLE,
    DOT_M2,
    DOT_ANDROID_STUDIO_STAR,
    JETBRAINS,
    ANDROID_STUDIO_STAR_APPDATA,
    ANDROID_STUDIO_STAR_LOCAL,
    ANDROID,
    TOTAL_STUDIO_FILE_PATHS
} StudioFilesPathIndex;

bool checkboxes[TOTAL_STUDIO_FILE_PATHS];

#define SDK_PATH "%LOCALAPPDATA%/Android"

#define USER_PROJECTS_PATH "%USERPROFILE%/AndroidStudioProjects"

void quietRemoveDir(LPCTSTR dir)  // Fully qualified name of the directory being deleted, without trailing backslash
{
    SHFILEOPSTRUCT file_op = {
        NULL,
        FO_DELETE,
        dir,
        "",
        FOF_CONFIRMMOUSE |
            FOF_NOERRORUI |
            FOF_SILENT,
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
    const float fontSize = 48.0f;
    const float spacing = 5.0f;

    const char *lines[] = {
        "ANDROID",
        "STUDIO",
        "PURGE"};
    const size_t totalLines = sizeof(lines) / sizeof(lines[0]);

    float longest = 0;
    float offsets[totalLines] = {};
    for (size_t i = 0; i < totalLines; i++) {
        float textPixelLength = MeasureTextEx(fonts[ROBOTO_SLAB_BOLD], lines[i], fontSize, spacing).x;
        longest = max(longest, textPixelLength);
        offsets[i] = textPixelLength;
    }

    for (size_t i = 0; i < totalLines; i++) {
        offsets[i] = (longest - offsets[i]) / 2;
        RL_DrawTextEx(fonts[ROBOTO_SLAB_BOLD], lines[i], (Vector2){marginLeft + offsets[i], marginTop + lineHeight * i}, fontSize, spacing, RAYWHITE);
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
    const float lines = floorf(SCROLLABLE_HEIGHT / SCROLLABLE_LINE_HEIGHT);
    const int fontSize = 20;
    const int spacing = 2;
    const char *textArr[TOTAL_STUDIO_FILE_PATHS] = {
        STUDIO_FILES_PATH};

    for (size_t i = 0; i < min(lines, TOTAL_STUDIO_FILE_PATHS); i++) {
        Vector2 rowAnchor = {scrollableAnchor.x + CHECKBOX_RESERVED_SPACE, scrollableAnchor.y + SCROLLABLE_LINE_HEIGHT * i - scrollOffset};
        const float textHeight = MeasureTextEx(fonts[0], textArr[i], fontSize, spacing).y;

        Vector2 textAnchor = rowAnchor;
        textAnchor.y += (SCROLLABLE_LINE_HEIGHT - textHeight) / 2;

        Vector2 boxAnchor = getRectAnchor(CHECKBOX_RESERVED_SPACE, CHECKBOX_RESERVED_SPACE, CHECKBOX_DIMENSIONS, CHECKBOX_DIMENSIONS, (Vector2){0, 0});
        boxAnchor.x += scrollableAnchor.x;
        boxAnchor.y += rowAnchor.y;

        RL_Rectangle boxDestRect = (RL_Rectangle){V2Unpack(boxAnchor), CHECKBOX_DIMENSIONS, CHECKBOX_DIMENSIONS};

        DrawTextureNPatch(textures[(checkboxes[i] ? BOX_FILLED : BOX_EMPTY)],
                          (NPatchInfo){0, 0, 152, 152, NPATCH_NINE_PATCH},
                          boxDestRect,
                          (Vector2){0, 0},
                          0.0f,
                          WHITE);

        RL_DrawTextEx(
            fonts[ROBOTO_SLAB_REGULAR],
            textArr[i],
            textAnchor,
            fontSize, spacing, RAYWHITE);
    }

    // Draw top and bottom wrapping rectangles
    DrawRectangle(scrollableAnchor.x, 0, SCROLLABLE_WIDTH, scrollableAnchor.y, BG_COLOR);
    DrawRectangle(scrollableAnchor.x, scrollableAnchor.y + SCROLLABLE_HEIGHT, SCROLLABLE_WIDTH, scrollableAnchor.y, BG_COLOR);

#if DEBUG
    // ? Draw Gizmos
    for (size_t i = 0; i < min(lines, TOTAL_STUDIO_FILE_PATHS); i++) {
        // Checkboxes
        Vector2 rowAnchor = {scrollableAnchor.x + CHECKBOX_RESERVED_SPACE, scrollableAnchor.y + SCROLLABLE_LINE_HEIGHT * i - scrollOffset};

        Vector2 boxAnchor = getRectAnchor(CHECKBOX_RESERVED_SPACE, CHECKBOX_RESERVED_SPACE, CHECKBOX_DIMENSIONS, CHECKBOX_DIMENSIONS, (Vector2){0, 0});
        boxAnchor.x += scrollableAnchor.x;
        boxAnchor.y += rowAnchor.y;

        RL_Rectangle boxDestRect = (RL_Rectangle){V2Unpack(boxAnchor), CHECKBOX_DIMENSIONS, CHECKBOX_DIMENSIONS};

        DrawRectangleLines(RectUnpack(boxDestRect), DEBUG_COLOR);

        // Text line bounding box
        DrawRectangleLines(
            V2Unpack(rowAnchor),
            SCROLLABLE_WIDTH - CHECKBOX_RESERVED_SPACE,
            SCROLLABLE_LINE_HEIGHT,
            DEBUG_COLOR);
    }

    DrawRectangleLines(
        V2Unpack(scrollableAnchor),
        SCROLLABLE_WIDTH,
        SCROLLABLE_HEIGHT,
        DEBUG_COLOR);
#endif
}

#define ScrollableRectContained(x, y) CheckContained((Vector2){x, y}, (RL_Rectangle){scrollableAnchor.x, scrollableAnchor.y, SCROLLABLE_WIDTH, SCROLLABLE_HEIGHT})

bool CheckContained(Vector2 point, RL_Rectangle space) {
    return (point.x >= space.x && point.x <= space.x + space.width) &&
           (point.y >= space.y && point.y <= space.y + space.height);
}

int getClickedRow(float x, float y) {
    assert(ScrollableRectContained(x, y) && "Invalid call of getClickedRow");
    return (int)floor((y - scrollableAnchor.y) / SCROLLABLE_LINE_HEIGHT);
}

void HandleClick(float x, float y) {
    if (ScrollableRectContained(x, y)) {
        checkboxes[getClickedRow(x, y)] ^= 1;
    }
}

void HandleScroll(float x, float y, float scroll) {
    if (ScrollableRectContained(x, y)) {
        scrollOffset += (scroll)*SCROLLABLE_LINE_HEIGHT;
        if (scrollOffset < 0) scrollOffset = 0;
    }
}

int main(void) {
    InitWindow(WIDTH, HEIGHT, "Android Studio Purge");
    SetTargetFPS(TARGET_FPS);
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

        float scroll = GetMouseWheelMoveV().y;
        if (scroll != 0) {
            HandleScroll(V2Unpack(GetMousePosition()), scroll);
        }

        EndDrawing();
    }

    UnloadResources();
    RL_CloseWindow();

    return 0;
}