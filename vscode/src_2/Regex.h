#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <iomanip>

 // フォントのサイズ
typedef struct{
    int Height;
    int Width;
}FontFormats;

/* std::stringやvectorはヒープ領域に確保されるため、戻り値として扱える */
class REGEX{
private:
    FontFormats font;
    void ArraytoString(const std::vector<uint8_t>& pixel,std::stringstream& ss,uint32_t columnBase);
    std::string RegexReplace(const std::string& before,const std::regex& pattern,const std::string& replacement);
public:
    REGEX(/* args */);
    ~REGEX();
    void SetFormat(int height,int width);
    /*** 正規表現置換で一つの配列にする ***/
    std::string EditFileString(const std::string& before,int width);
    /*** ファイル内テキストの整数化 ***/
    std::vector<uint16_t> StringtoHex(const std::string& file);
    /*** SSD130x用フォントに編集 ***/
    std::string FontReverse(const std::vector<uint16_t>& data,const std::string& name);
};



