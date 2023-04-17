#include "Wad.h"

// ===============================================
// FileSystemObj Class Function Definitions
// ===============================================
FileSystemObj::FileSystemObj() {}


FileSystemObj::FileSystemObj(char* name, bool isDirectory, char* content) {
    this->name = name;
    this->isDirectory = isDirectory;
    this->content = content;
}


void FileSystemObj::appendChild(FileSystemObj* child) {}

// ===============================================
// Wad Function Class Definitions
// ===============================================
FileSystemObj* Wad::parseFile(ifstream &file) {}


FileSystemObj* Wad::parseMap(ifstream &file) {}


FileSystemObj* Wad::parseNamespace(ifstream &file) {}


Wad::Wad() {}


Wad* Wad::loadWad(const string& path) {
    Wad* wad = new Wad();
    ifstream inputFile(path, ios::binary);

    if (!inputFile.is_open()) throw runtime_error("Cannot open file");

    // read magic
    inputFile.read(wad->magic, 4);

    // read descriptor metadata
    unsigned int numDescriptors, descriptorOffset;
    inputFile.read((char*)&numDescriptors, 4);
    inputFile.read((char*)&descriptorOffset, 4);

    // read and parse elements
    for (int i = 0; i < numDescriptors; i++) {
        inputFile.seekg(descriptorOffset + i * 16, ios_base::beg);
        unsigned int elemOffset = 0, elemLen = 0;
        char* elemName = new char[8];

        // read metadata
        inputFile.read((char*)&elemOffset, 4);
        inputFile.read((char*)&elemLen, 4);
        inputFile.read(elemName, 8);

        // read file content
        inputFile.seekg(elemOffset, ios_base::beg);
        char* content = new char[elemLen];
        inputFile.read(content, elemLen);

        for (int j = 0; j < 8; j++) {
            cout << elemName[j];
        }
        cout << " : ";
        for (int j = 0; j < elemLen; j++) {
            cout << content[j];
        }
        cout << endl;

        delete[] elemName;
        delete[] content;
    }

    inputFile.close();
    return wad;
}


string Wad::getMagic() {}


bool Wad::isContent(const string &path) {}


bool Wad::isDirectory(const string &path) {}


int Wad::getSize(const string &path) {}


int Wad::getContents(const string &path, char *buffer, int length, int offset = 0) {}


int Wad::getDirectory(const string &path, vector<string> *directory) {}
