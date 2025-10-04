#include <iostream>
#include <iomanip>
#include <clocale>
#include <filesystem>
#include <string>
#include <windows.h>
#include <conio.h>
#include <fstream>

#include "consoleCommands.h"
#include "keyboardMenuControll.h"
#include "fileManagerCommands.h"


using namespace std;

int main()
{
    setlocale(LC_ALL, "ukr");
    ShowConsoleCursor(false);

    // current directory
    filesystem::path path = filesystem::current_path();
    // amount of elements in the directory

    int elementsCount = getDirectoryEntriesCount(path);

    int active = 0;
    string newFolderName;

    filesystem::path copyPath;
    bool deleteCopyPathFile = false;

    while (true) {
        
        drawDirectoryContents(path, active);
        // control menu up and down and entity deleting
        int key = _getch();
        if (key == 224) {
            key = _getch();
            if (key == 72) {
                if (active > 0) active--; // up
            }
            else if (key == 80) {
                if (active < elementsCount - 1)  active++; // down
            }

            // delete something
            else if (key == DELETE_KEY) {
                bool reallyDelete = deleteMenuChoice(10, 5, 40, 6);

                if (reallyDelete) deleteEntity(path, active, elementsCount);
                else continue;
                
            }
        }
        else if (key == ENTER_KEY) {
            
            int index = 0;
            system("cls");
            if (active == 0) {
                if (path != path.root_path()) {
                    system("cls");
                    path = path.parent_path();
                    elementsCount = getDirectoryEntriesCount(path);
                    continue;
                }
                else {
                    path = "";
                    system("cls");
                    string disks[26];
                    int count;
                    listAllTheDisks(disks, count);

                    const char* cDisks[26];
                    for (int i = 0; i < count; i++) {
                        cDisks[i] = disks[i].c_str();
                    }
                    cout << "Current path: This PC" << endl;
                    cout << string(path.string().length() + 14, '-') << endl;

                    int choice = menuControl(cDisks, count, 0, 2);

                    path = disks[choice - 1];
                    system("cls");
                    path = path.parent_path();
                    elementsCount = getDirectoryEntriesCount(path);
                }
                
            }
            
            index++;
                
            for (const filesystem::directory_entry& entry : filesystem::directory_iterator(path))
            {
                // If the current index matches the active selection
                if (index == active) {
                    if (entry.is_directory()) {
                        path = entry.path();
                        active = 0;
                        elementsCount = getDirectoryEntriesCount(path);
                    }
                    break;
                }
                index++;
            }
            
        }
        else if (key == BACKSPACE_KEY) {
            system("cls");
            path = path.parent_path();
            active = 0;
            elementsCount = getDirectoryEntriesCount(path);
        }
        else if (key == CNTL_N_KEY) {
            // get the folder name
            SetCursorPosition(startMenuPrintX, infoPrintPosY);
            ShowConsoleCursor(true);
            cout << "Enter the name of the folder: "; getline(cin, newFolderName);
            ShowConsoleCursor(false);
            // create a folder
            filesystem::create_directories(path / newFolderName);

            // redraw the filesystem
            system("cls");
            active = 0;
            elementsCount = getDirectoryEntriesCount(path);
        }
        else if (key == CNTL_O_KEY) {
            SetCursorPosition(startMenuPrintX, infoPrintPosY);
            int index = 1;
            for (const filesystem::directory_entry& entry : filesystem::directory_iterator(path))
            {
                const int fileMenuSize = 2;
                const char* controlMenuFile[fileMenuSize] = {
                    "Cntl + F - find the word repetitions in the file",
                    "Cntl + R - replace the particular word in the file"
                };
                if (index == active) {
                    if (!entry.is_directory()) {
                        // read the full path
                        filesystem::path fullFilePath = entry.path();
                        
                        // proceed it
                        ShowConsoleCursor(true);
                        string fileData = readDataFromFile(fullFilePath.string());
                        system("cls");
                        cout << fileData;
                        printFileMenu(controlMenuFile, fileMenuSize);
                        while (key != 13) {
                            key = _getch();
                            if (key == CNTL_F_KEY) {
                                string word;
                                SetCursorPosition(startMenuPrintX, startMenuPrintY + fileMenuSize + 2);
                                cout << "Enter the word to find: "; getline(cin, word);
                                int wordRepetitions = findMatchingWordsFile(entry.path().string(), word);

                                cin.get();
                                system("cls");
                                cout << fileData;
                                printFileMenu(controlMenuFile, fileMenuSize);
                            }
                            else if (key == CNTL_R_KEY) {
                                string word, wordToReplaceWith;
                                SetCursorPosition(startMenuPrintX, startMenuPrintY + fileMenuSize + 2);
                                cout << "Enter the word to replace: "; getline(cin, word);
                                SetCursorPosition(startMenuPrintX, startMenuPrintY + fileMenuSize + 3);
                                cout << "Enter the word to replace with: "; getline(cin, wordToReplaceWith);
                                findAndReplaceWordsFile(entry.path().string(), word, wordToReplaceWith);

                                //reload file
                                ifstream file(entry.path());
                                string fileData;
                                if (file.is_open()) {
                                    string line;
                                    while (getline(file, line)) {
                                        fileData += line + "\n";
                                    }
                                    file.close();
                                }

                                system("cls");
                                cout << fileData;
                                printFileMenu(controlMenuFile, fileMenuSize);
                            }
                        }
                        cin.get();
                        ShowConsoleCursor(false);
                        system("cls");
                        break;
                        
                    }
                }
                
                index++;
            }
        
        }
        else if (key == CNTL_C_KEY) {
            int index = 1;
            for (const filesystem::directory_entry& entry : filesystem::directory_iterator(path))
            {

                if (index == active) {
                    copyPath = entry.path();
                    break;
                    
                }
                index++;
            }
            deleteCopyPathFile = false;
        }
        else if (key == CNTL_V_KEY) {
            copyFileTo(copyPath, path, deleteCopyPathFile);
            
            system("cls");
            elementsCount = getDirectoryEntriesCount(path);
        }
        else if (key == CNTL_X_KEY) {
            int index = 1;
            for (const filesystem::directory_entry& entry : filesystem::directory_iterator(path))
            {

                if (index == active) {
                    copyPath = entry.path();
                    break;

                }
                index++;
            }
            deleteCopyPathFile = true;
        }
        else if (key == CNTL_L_KEY) {
            SetCursorPosition(startMenuPrintX, infoPrintPosY);
            cout << "Choose what do you want to search: ";
            const int searchMenuSize = 2;
            const char* searchMenu[searchMenuSize] = {
                "Search by mask", // choice 1
                "Search by name", // choice 2
            };

            int choice = menuControl(searchMenu, searchMenuSize, startMenuPrintX, infoPrintPosY + 1);
            
            if (choice == 1) {
                string targetExtension;
                SetCursorPosition(startMenuPrintX, infoPrintPosY + 3);
                ShowConsoleCursor(true);
                cout << "Enter the extension of a file with a dot: "; getline(cin, targetExtension);
                findByFileExtension(targetExtension, path, startMenuPrintX, infoPrintPosY + 5);
                cin.get();
                ShowConsoleCursor(false);
                system("cls");
            }
            else if (choice == 2) {
                string targetFile;
                SetCursorPosition(startMenuPrintX, infoPrintPosY + 3);
                ShowConsoleCursor(true);
                cout << "Enter the name of the file to search with an extention: "; getline(cin, targetFile);
                findFileByName(targetFile, path, startMenuPrintX, infoPrintPosY + 5);
                cin.get();
                ShowConsoleCursor(false);
                system("cls");
            }
            
            
        }
    }

}


