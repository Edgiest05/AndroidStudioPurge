#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../raylib/src/raylib.h"


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

#include <windows.h>
#include <process.h>

void quietRemoveDir(LPCTSTR dir) // Fully qualified name of the directory being deleted, without trailing backslash
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
        "" };
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
    DIR* dir = opendir("/opt/android-studio");
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

int main(void) {
    InitWindow(800, 450, "Android Studio Purge");

    while (!WindowShouldClose())
    {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            RL_DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    RL_CloseWindow();

    return 0;
}