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

// print all of the disks
void listAllTheDisks(string disks[], int& count) {
    DWORD drives = GetLogicalDrives();
    count = 0;

    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        if (drives & (1 << (letter - 'A'))) {
            string root = string(1, letter) + ":\\";
            try {
                if (filesystem::exists(root)) {
                    /*cout << root << "\n";*/
                    disks[count++] = root;
                }
            }
            catch (const filesystem::filesystem_error& e) {
                cerr << "Error accessing " << root << ": " << e.what() << "\n";
            }
        }
    }

}


// Вивід вмісту директорії
void drawDirectoryContents(const filesystem::path& path, int activeRow)
{
    SetCursorPosition(0, 0);
    cout << "Current path: " << path.string() << endl;
    cout << string(path.string().length() + 14, '-') << endl;

    int col1Width = 40;
    int col2Width = 10;
    int col3Width = 20;

    int index = 0;
    bool isRoot = false;


    const char* controlMenu[menuSize] = {
        "Arrow down - move down\n",
        "Arrow up - move up\n",
        "Enter - enter directory\n",
        "Backspace - move to parent directory\n",
        "Cntl + N - create new directory\n",
        "Cntl + O - open the file\n",
        "Cntl + X - cut the file\n",
        "Cntl + C - copy the file\n",
        "Cntl + V - paste the file\n",
        "Cntl + L - Look for file\n"
    };


    if (!path.empty())
    {

        isRoot = false;
        if (index == activeRow) {
            SetColor(BLACK, WHITE);
        }

        cout << left << setw(col1Width) << ".." << setw(col2Width) << "UP" << endl;
        SetColor(WHITE, BLACK);

        index++;
    }


    for (const filesystem::directory_entry& entry : filesystem::directory_iterator(path))
    {
        if (index == activeRow) {
            SetColor(BLACK, WHITE);
        }
        else {
            SetColor(WHITE, BLACK);
        }
        // Ім'я файлу
        cout << left << setw(col1Width) << entry.path().filename().string();
        // Файл чи директорія
        if (entry.is_directory()) {
            cout << "<DIR>";
        }
        else {
            cout << setw(col2Width) << filesystem::file_size(entry);
        }
        //filesystem::file_time_type lastWriteTime = filesystem::last_write_time(path);
        //cout << lastWriteTime;

        SetColor(WHITE, BLACK);
        cout << endl;

        index++;

    }

    for (int i = 0; i < menuSize; ++i) {
        SetCursorPosition(startMenuPrintX, startMenuPrintY + i);
        cout << controlMenu[i];
    }

}

int getDirectoryEntriesCount(const filesystem::path& path)
{
    auto count = distance(filesystem::directory_iterator(path), filesystem::directory_iterator{});
    if (!path.parent_path().empty()) {
        count++; // For the ".." entry
    }
    return count;
}

string readDataFromFile(string filePath) {
    ifstream file(filePath);
    string str, file_contents;

    if (file.is_open()) {
        while (getline(file, str)) {
            file_contents += str;
            file_contents.push_back('\n');
        }
    }

    return file_contents;
}

void copyFileTo(filesystem::path from, filesystem::path to, bool deleteCopyPathFile) {


    if (from.string() != "") {
        if (filesystem::is_directory(from)) { // directory check cause directories don't have extensions
            filesystem::create_directories(to / from.filename().string());
            filesystem::copy(from, to / from.filename(), filesystem::copy_options::overwrite_existing);
        }
        else {
            filesystem::copy(from, to);
        }

    }
    if (deleteCopyPathFile) filesystem::remove_all(from);

}

void findFileByName(string targetFile, filesystem::path directory, int startPosX, int startPosY) {
    int y = startPosY;
    int filesFound = 0;
    for (const auto& entry : filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().filename() == targetFile) {
            SetCursorPosition(startMenuPrintX, y);
            ++y;
            cout << "Found: " << entry.path() << endl;
            ++filesFound;
        }
    }

    if (filesFound == 0) {
        SetCursorPosition(startMenuPrintX, y);
        cout << "No files has been found with matching the name \"" << targetFile << "\" \n";
    }
}


void findByFileExtension(string targetExtension, filesystem::path directory, int startPosX, int startPosY) {
    int y = startPosY;
    int filesFound = 0;
    for (const auto& entry : filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == targetExtension) {
            SetCursorPosition(startMenuPrintX, y);
            ++y;
            cout << "Found: " << entry.path() << endl;
            ++filesFound;
        }
    }

    if (filesFound == 0) {
        SetCursorPosition(startMenuPrintX, y);
        cout << "No files has been found with matching the name \"" << targetExtension << "\" \n";
    }
}

int findMatchingWordsFile(const string& filepath, string word) {
    string str;
    int lineNumber = 0;
    int wordsCount = 0;

    // lowercase the search word
    transform(word.begin(), word.end(), word.begin(), ::tolower);
    ifstream file(filepath);
    if (file.is_open()) {
        while (getline(file, str)) {
            lineNumber++;

            // make a lowercase copy of the line
            string lowerStr = str;
            transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);

            if (lowerStr.find(word) != string::npos) {
                cout << "'" << word << "' found at line: " << lineNumber << endl;
                wordsCount++;
            }
        }
    }
    return wordsCount;
}

void findAndReplaceWordsFile(const string& filepath, string wordToReplace, string wordToReplaceWith) {
    string str;
    int lineNumber = 0;
    string fileContent;

    // lowercase the search word
    transform(wordToReplace.begin(), wordToReplace.end(), wordToReplace.begin(), ::tolower);
    ifstream file(filepath);
    if (file.is_open()) {
        while (getline(file, str)) {
            lineNumber++;

            // make a lowercase copy of the line
            string lowerStr = str;
            transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);

            int wordLength = wordToReplace.length();
            int pos = lowerStr.find(wordToReplace);
            if (pos != string::npos) {
                str.replace(pos, wordLength, wordToReplaceWith);
            }
            fileContent += str += '\n';
        }
        file.close();

        ofstream file(filepath);
        if (file.is_open()) {
            file << fileContent;
        }
        file.close();
    }
}

void printFileMenu(const char* controlMenuFile[], const int fileMenuSize) {
    for (int i = 0; i < fileMenuSize; ++i) {
        SetCursorPosition(startMenuPrintX, startMenuPrintY + i);
        cout << controlMenuFile[i] << endl;
    }
}

void deleteEntity(filesystem::path& path, int& active, int& elementsCount) {
    // don't include ".." entry to delete properly
    int index = 1;

    for (const filesystem::directory_entry& entry : filesystem::directory_iterator(path))
    {
        // If the current index matches the active selection
        if (index == active) {
            if (entry.is_directory()) {

                // remove
                filesystem::remove_all(entry.path());
                // redraw the filesystem
                system("cls");
                active = 0;
                elementsCount = getDirectoryEntriesCount(path);
            }
            else {
                // remove
                filesystem::remove(entry.path());
                // redraw the filesystem
                system("cls");
                active = 0;
                elementsCount = getDirectoryEntriesCount(path);
            }
            break;
        }
        index++;
    }
}
