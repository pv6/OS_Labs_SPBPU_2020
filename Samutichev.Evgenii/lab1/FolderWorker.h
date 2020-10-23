#ifndef FOLDERWORKER_H_INCLUDED
#define FOLDERWORKER_H_INCLUDED

#include <string>

class FolderWorker {
public:
    FolderWorker(const std::string& folder1Path, const std::string& folder2Path, size_t oldDefTime);

    void work();

private:
    std::string _folder1Path;
    std::string _folder2Path;
    size_t _oldDefTime;

    bool dirExists(const std::string& folderPath) const;
};

#endif // FOLDERWORKER_H_INCLUDED
