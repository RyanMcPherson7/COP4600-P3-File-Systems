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
        string getName();
        string getContent();
        vector<string> getChildrenNames();
        vector<FileSystemObj*> getChildren();
};

class Wad {
    private:
        string magic;
        FileSystemObj* root;
        vector<string> parsePath(const string& path);
        FileSystemObj* getDestinationEntity(const string& path);
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