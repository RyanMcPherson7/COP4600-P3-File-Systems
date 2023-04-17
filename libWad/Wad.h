#pragma once
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

class FileSystemObj {
    private:
        char* name;
        vector<FileSystemObj*> children;
        bool isDirectory;
        char* content;
        unsigned int contentSizeInBytes;
    public:
        FileSystemObj();
        FileSystemObj(char* name, bool isDirectory, char* content);
        void appendChild(FileSystemObj* child);
};

class Wad {
    private:
        char* magic;
        FileSystemObj* root;
        FileSystemObj* parseFile(ifstream &file);
        FileSystemObj* parseMap(ifstream &file);
        FileSystemObj* parseNamespace(ifstream &file);
    public:
        Wad();
        static Wad* loadWad(const string& path);
        string getMagic();
        bool isContent(const string &path);
        bool isDirectory(const string &path);
        int getSize(const string &path);
        int getContents(const string &path, char *buffer, int length, int offset);
        int getDirectory(const string &path, vector<string> *directory);
};