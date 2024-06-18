#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <windows.h>

void move_file(const char *src, const char *dest) {
    if (CopyFile(src, dest, FALSE) == 0) {
        printf("Kopirování souboru %s do %s selhalo. Chyba: %lu\n", src, dest, GetLastError());
    } else {
        printf("Soubor %s byl přesunut do %s\n", src, dest);
    }
}

void get_home_directory(char *home_path, size_t max_length) {
    const char *user_profile = getenv("USERPROFILE");
    if (user_profile) {
        snprintf(home_path, max_length, "%s", user_profile);
    } else {
        fprintf(stderr, "Nelze získat cestu k domovskému adresáři.\n");
        exit(1);
    }
}

int main() {
    const char *folderA = ".\\files";
    char folderB[512];

    char home_directory[512];
    get_home_directory(home_directory, sizeof(home_directory));

    snprintf(folderB, sizeof(folderB), "%s\\AppData\\Roaming\\.minecraft\\mods", home_directory);

    if (_mkdir(folderB) != 0) {
        if (errno != EEXIST) {
            perror("Vytvoření složky B selhalo");
            return 1;
        }
    }

    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(strcat(strdup(folderA), "\\*"), &findFileData);
    
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Chyba při otevírání složky A\n");
        return 1;
    } else {
        do {
            const char *file_name = findFileData.cFileName;
            if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0) {
                continue;
            }

            char src_path[512];
            char dest_path[512];
            snprintf(src_path, sizeof(src_path), "%s\\%s", folderA, file_name);
            snprintf(dest_path, sizeof(dest_path), "%s\\%s", folderB, file_name);

            move_file(src_path, dest_path);
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);
    }

    ShellExecute(NULL, "open", ".\\fabric-installer-1.0.1.exe", NULL, NULL, SW_SHOWNORMAL);

    return 0;
}
