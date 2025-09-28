#pragma once

#define startMenuPrintX 55
#define startMenuPrintY 3
const int menuSize = 10;
#define infoPrintPosY  startMenuPrintY + menuSize + 1

#define ENTER_KEY 13
#define BACKSPACE_KEY 8
#define DELETE_KEY 83
#define CNTL_A_KEY 1
#define CNTL_B_KEY 2
#define CNTL_C_KEY 3
#define CNTL_D_KEY 4
#define CNTL_E_KEY 5
#define CNTL_F_KEY 6
#define CNTL_G_KEY 7
#define CNTL_H_KEY 8
#define CNTL_I_KEY 9
#define CNTL_G_KEY 10
#define CNTL_K_KEY 11
#define CNTL_L_KEY 12
#define CNTL_M_KEY 13
#define CNTL_N_KEY 14
#define CNTL_O_KEY 15
#define CNTL_P_KEY 16
#define CNTL_Q_KEY 17
#define CNTL_R_KEY 18
#define CNTL_S_KEY 19
#define CNTL_T_KEY 20
#define CNTL_U_KEY 21
#define CNTL_V_KEY 22
#define CNTL_W_KEY 23
#define CNTL_X_KEY 24
#define CNTL_Y_KEY 25
#define CNTL_Z_KEY 26

void listAllTheDisks(string disks[], int& count);
void drawDirectoryContents(const filesystem::path& path, int activeRow);
int getDirectoryEntriesCount(const filesystem::path& path);
string readDataFromFile(string filePath);
void copyFileTo(filesystem::path from, filesystem::path to, bool deleteCopyPathFile);
void findFileByName(string targetFile, filesystem::path directory, int startPosX, int startPosY);
void findByFileExtension(string targetExtension, filesystem::path directory, int startPosX, int startPosY);
int findMatchingWordsFile(const string& filepath, string word);
void findAndReplaceWordsFile(const string& filepath, string wordToReplace, string wordToReplaceWith);
void printFileMenu(const char* controlMenuFile[], const int fileMenuSize);
void deleteEntity(filesystem::path& path, int& active, int& elementsCount);