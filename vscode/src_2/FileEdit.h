#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <sstream>
#include <regex>
#include <vector>
#include <filesystem>

typedef struct {
    std::string BdfFileName;
    std::string FontFile;
    int StartCode;
    int EndCode;
}UserInputFiles;

class FILEEDIT
{
private:
    /* data */
public:
    FILEEDIT(/* args */);
    ~FILEEDIT();
    bool ArgumentCheck(std::string& Filename);
    bool CheckBdfFile(const std::string& Filename,int& x,int& y,int& code);

    std::string InputFileRead(const std::string& filename);
    std::string OutputFileWrite(const std::string& filename,const std::string& After);
};


