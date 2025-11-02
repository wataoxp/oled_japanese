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

/* BDFtoBYTE */
// 状態管理フラグ
enum class State{
    SEARCH_JISCODE,
    START_BITMAP,
    WAITING_ENDCHAR,
    FINISH_CONVERT,
};

class BDF{
private:
    // 文字列をバイト配列として出力
    void PushArray(const std::string& hex_data_line, std::vector<unsigned char>& output_array);
    bool CheckString(const std::string& line,const std::string& targetString);
    uint8_t ChartoHex(const std::string& str);
public:
    BDF(/* args */);
    ~BDF();
   
    // BDFファイルの走査
    std::optional<std::vector<unsigned char>> ConvertBDFtoArray(const std::string& filename,const std::string& jiscode);
    // バイト配列の作成
    std::string ExportByteArray(const std::vector<unsigned char>& data,const std::string& JisCode);
    std::string HextoString(int code,int num);
};

inline std::string BDF::HextoString(int code,int num)
{
    std::stringstream tmp;

    tmp << std::hex;
    tmp << code + num;

    return tmp.str();
}

inline bool BDF::CheckString(const std::string& line,const std::string& targetString)
{    
    return line.rfind(targetString) == 0;       //0であるときtrue
}

// 16進数文字列 (例: "40") を1バイト (0x40) に変換するヘルパー関数
inline uint8_t BDF::ChartoHex(const std::string& str)
{
    uint8_t hex = static_cast<uint8_t>(std::stoi(str,nullptr,16));

    return hex;
}

