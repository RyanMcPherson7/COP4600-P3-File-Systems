#pragma once
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

class FileSystemObj {
    private:
        string name;
        vector<FileSystemObj*> children;
        string content;
    public:
        FileSystemObj();
        FileSystemObj(string name, string content);
        void appendChild(FileSystemObj* child);
        bool isMap();
        int getNumChildren();
};

class Wad {
    private:
        string magic;
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