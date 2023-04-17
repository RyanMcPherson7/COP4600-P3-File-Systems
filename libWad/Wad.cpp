#include "Wad.h"
#include <stack>
#include <cctype>

// ===============================================
// FileSystemObj Class Function Definitions
// ===============================================
FileSystemObj::FileSystemObj() {}


FileSystemObj::FileSystemObj(string name, string content) {
    this->name = name;
    this->content = content;
}


void FileSystemObj::appendChild(FileSystemObj* child) {
    this->children.push_back(child);
}


bool FileSystemObj::isMap() {
    return name.length() == 4 && name.at(0) == 'E' && 
        isdigit(name.at(1)) && name.at(2) == 'M' && isdigit(name.at(3));
}


int FileSystemObj::getNumChildren() {
    return this->children.size();
}


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
    char* charMagic = new char[4];
    inputFile.read(charMagic, 4);
    wad->magic = (string(charMagic)).substr(0, 4);

    // read descriptor metadata
    unsigned int numDescriptors, descriptorOffset;
    inputFile.read((char*)&numDescriptors, 4);
    inputFile.read((char*)&descriptorOffset, 4);

    // use stack to track directory nesting
    stack<FileSystemObj*> hierarchy;
    FileSystemObj* root = new FileSystemObj("root", "");
    hierarchy.push(root);

    // read and parse elements
    for (int i = 0; i < numDescriptors; i++) {
        inputFile.seekg(descriptorOffset + i * 16, ios_base::beg);
        unsigned int elemOffset = 0, elemLen = 0;
        char* charElemName = new char[8];

        // read metadata
        inputFile.read((char*)&elemOffset, 4);
        inputFile.read((char*)&elemLen, 4);
        inputFile.read(charElemName, 8);
        string elemName(charElemName);
        elemName = elemName.substr(0, 8);
        delete[] charElemName;

        // check if map and 10 element limit reached
        if (hierarchy.top()->isMap() && hierarchy.top()->getNumChildren() == 10) {
            hierarchy.pop();
        }

        // determine if file, namespace, or map
        if (elemLen != 0) { 
            // read file content
            inputFile.seekg(elemOffset, ios_base::beg);
            char* charFileContent = new char[elemLen];
            inputFile.read(charFileContent, elemLen);
            string fileContent(charFileContent);
            fileContent = fileContent.substr(0, elemLen);
            delete[] charFileContent;

            FileSystemObj* newFile = new FileSystemObj(elemName, fileContent);
            hierarchy.top()->appendChild(newFile);

        } 
        else if (elemName.find("_START") != string::npos) { // opening namespace
            elemName = elemName.substr(elemName.find("_START"));
            FileSystemObj* newNamespace = new FileSystemObj(elemName, "");
            hierarchy.top()->appendChild(newNamespace);
            hierarchy.push(newNamespace);

        } else if (elemName.find("_END") != string::npos) { // closing namespace
            hierarchy.pop();

        } else { // map
            FileSystemObj* newMap = new FileSystemObj(elemName, "");
            hierarchy.top()->appendChild(newMap);
            hierarchy.push(newMap);
        }
    }

    wad->root = root;

    inputFile.close();
    return wad;
}


string Wad::getMagic() {
    return this->magic;
}


bool Wad::isContent(const string &path) {}


bool Wad::isDirectory(const string &path) {}


int Wad::getSize(const string &path) {}


int Wad::getContents(const string &path, char *buffer, int length, int offset = 0) {}


int Wad::getDirectory(const string &path, vector<string> *directory) {}
