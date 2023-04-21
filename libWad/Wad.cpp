#include "Wad.h"
#include <stack>
#include <cctype>
#include <sstream>

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


string FileSystemObj::getName() {
    return this->name;
}


string FileSystemObj::getContent() {
    return this->content;
}


vector<string> FileSystemObj::getChildrenNames() {
    vector<string> childrenNames;

    for (FileSystemObj* child : this->children) {
        childrenNames.push_back(child->getName());
    }

    return childrenNames;
}


vector<FileSystemObj*> FileSystemObj::getChildren() {
    return this->children;
}


// ===============================================
// Wad Function Class Definitions
// ===============================================
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
    FileSystemObj* root = new FileSystemObj("", "");
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
            string fileContent = "";
            for (int i = 0; i < elemLen; i++) {
                fileContent += charFileContent[i];
            }

            delete[] charFileContent;

            FileSystemObj* newFile = new FileSystemObj(elemName, fileContent);
            hierarchy.top()->appendChild(newFile);

        } 
        else if (elemName.find("_START") != string::npos) { // opening namespace
            elemName = elemName.substr(0, elemName.find("_START"));
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


bool Wad::isContent(const string &path) {
    FileSystemObj* destinationEntity = getDestinationEntity(path);

    if (destinationEntity == nullptr) return false;

    return destinationEntity->getContent().length() > 0;
}


bool Wad::isDirectory(const string &path) {
    return !isContent(path);
}


int Wad::getSize(const string &path) {
    FileSystemObj* destinationEntity = getDestinationEntity(path);

    if (destinationEntity == nullptr) return -1;

    int contentSize = destinationEntity->getContent().length();

    return contentSize > 0 ? contentSize : -1; 
}


int Wad::getContents(const string &path, char *buffer, int length, int offset = 0) {
    FileSystemObj* destinationEntity = getDestinationEntity(path);

    if (destinationEntity == nullptr) return -1;

    string content = destinationEntity->getContent();

    if (content.length() == 0) {
        return -1;
    }

    if (length > content.length()) {
        length = content.length();
    }
    content = content.substr(offset, length);

    for (int i = 0; i < length; i++) {
        buffer[i] = content.at(i);
    }

    return length;
}


int Wad::getDirectory(const string &path, vector<string> *directory) {
    FileSystemObj* destinationEntity = getDestinationEntity(path);

    if (destinationEntity == nullptr) return -1;

    string content = destinationEntity->getContent();


    if (content.length() > 0) {
        return -1;
    }

    // copy over children
    vector<string> childrenNames = destinationEntity->getChildrenNames();
    for (string childName : childrenNames) {
        directory->push_back(childName);
    }
    
    return destinationEntity->getNumChildren();
}


FileSystemObj* Wad::getDestinationEntity(const string& path) {
    vector<string> entityNames;
    stringstream stream(path);
    string name;

    // parse path
    while (getline(stream, name, '/')) {
        entityNames.push_back(name);
    }

    // root path requested
    if (path.length() == 1) {
        return this->root;
    }

    // change directories into destination
    FileSystemObj* destination = this->root;
    for (int i = 1; i < entityNames.size(); i++) {
        vector<FileSystemObj*> children = destination->getChildren();
        bool destinationChange = false;

        for (FileSystemObj* child : children) {
            if (child->getName().compare(entityNames.at(i)) == 0) {
                destination = child;
                destinationChange = true;
                break;
            }
        }

        // invalid file path
        if (!destinationChange) {
            return nullptr;
        }
    }

    return destination;
}
