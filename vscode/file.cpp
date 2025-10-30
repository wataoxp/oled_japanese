#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <iomanip>

/*** ファイルの編集(正規表現) ****/
/* std::stringやvectorはヒープ領域に確保されるため、戻り値として扱える */
std::string EditFileString(const std::string& before);
std::string InputFileOpen(const std::string& filename);
std::string RegexReplace(const std::string& before,const std::regex& pattern,const std::string& replacement);
std::string OutputFileWrite(const std::string& filename,const std::string& After);

/*** ファイル内テキストの整数化 ***/
std::vector<uint16_t> StringtoHex(const std::string& file);
/*** SSD130x用フォントに編集 ***/
std::string FontReverse(const std::vector<uint16_t>& data,const std::string& name);
/*** 反転した数値を文字列に格納 ***/
inline void ArraytoString(const std::vector<uint8_t>& pixel,std::stringstream& ss,uint32_t columnBase);

// フォントのサイズ
constexpr uint32_t FontHeight = 12;
constexpr uint32_t FontWidth = 12;

int main(void) 
{
    std::string before,after;
    std::string InputFile = "shinonome_area_1.h";
    std::string tmpOutputFile = "tmp12bit.h";
  
    std::string Arrayname = "kigou";

    std::string OutputFile;
    OutputFile = ""+Arrayname+".h";

    before = InputFileOpen(InputFile);

    if(before == "Error")
    {
        std::cerr << "入力ファイル開封エラー:" << InputFile << std::endl;
        return 1;
    }

    after = EditFileString(before);
    
    if(OutputFileWrite(tmpOutputFile,after) == "Error")
    {
        std::cerr << "出力ファイル開封エラー:" << tmpOutputFile << std::endl;
        return 1;
    }

    std::cout << "一時ファイル出力:" << tmpOutputFile << std::endl;

    std::vector<uint16_t> data = StringtoHex(after);
    after = FontReverse(data,Arrayname);

    if(OutputFileWrite(OutputFile,after) == "Error")
    {
        std::cerr << "出力ファイル開封エラー:" << OutputFile << std::endl;
        return 1;
    }

    std::cout << "ファイル出力完了:" << OutputFile << std::endl;

    return 0;
}

std::string EditFileString(const std::string& before)
{
    std::string str = before;   //置換前の文字列
    std::regex pattern;         //置換対象の正規表現を記述
    std::string replacement;    //置換する文字列
    std::string After;          //置換後の文字列                   

    pattern = R"(unsigned char .+\[\] = |,0x)";         //R”()”は正規表現用リテラル
    replacement = "";
    After = RegexReplace(str,pattern,replacement);

    pattern = R"(\n\{\n|\{\n)";                         //正規表現はOR結合が可能
    replacement = "{";
    After = RegexReplace(After,pattern,replacement);    //2回目以降は編集後の文字列を渡す

    pattern = R"(,\n)";
    replacement = ",";
    After = RegexReplace(After,pattern,replacement);

    pattern = R"(\n\};)";
    replacement = "},";
    After = RegexReplace(After,pattern,replacement);

    pattern = R"([ \t])";                               //\sは改行も含んでしまう
    replacement = "";
    After = RegexReplace(After,pattern,replacement);

    pattern = R"(\{)";                                  //インデント
    replacement = "\t{";
    After = RegexReplace(After,pattern,replacement);

    return After;
}

// 編集するファイルを開く
std::string InputFileOpen(const std::string& filename)
{
    std::string result = "Error";

    std::ifstream ifs(filename);
    if(!ifs.is_open())
    {
        return result;
    }

    // ファイルの内容をすべて読み込む
    std::stringstream buffer;
    // ファイルの内容全体を一つの文字列ストリームに読み込み、そこからstringに変換
    buffer << ifs.rdbuf();

    // 読み込んだファイルの内容を格納
    result = buffer.str();

    return result;
}

//文字列を置換する
std::string RegexReplace(const std::string& before,const std::regex& pattern,const std::string& replacement)
{
    std::string content = before;
    content = std::regex_replace(content,pattern,replacement);      //置換後の文字列を格納

    return content;
}

//出力ファイルを開く(無ければ新規作成)
std::string OutputFileWrite(const std::string& filename,const std::string& After)
{
    std::string result = "Error";

    std::ofstream ofs(filename);
    if(!ofs.is_open())
    {
        return result;
    }
    result = "Success";

    ofs << After;       //引数の文字列を出力ファイルに書き込む

    return result;
}

std::vector<uint16_t> StringtoHex(const std::string& file)
{
    // 文字列を<X>の型で格納する
    std::vector<uint16_t> FontData;
    // 16進数のパターンを検索
    std::regex HexRegex("0x([0-9a-fA-F]+)");
    // std::sregex_iterator でファイル全体からパターンに一致する全てを列挙
    std::sregex_iterator it(file.begin(),file.end(),HexRegex);
    // std::sregex_iteratorオブジェクトの終了を定義
    std::sregex_iterator end;

    // HexRagexに該当した文字列を格納(1要素分)
    std::string hex_str;
    // hex_strを16進数として格納
    uint16_t value;

    // itが文字列の終点を指したとき、itはendと同じ値を指す
    for (; it != end; it++)
    {
        // (*it)[0].str() はマッチした文字列全体（例: "0xABCD"）
        hex_str = (*it)[0].str();

        // 文字列（テキスト）を16進数として読み込み、uint16_t型の数値に変換
        // nullptr: エラーチェック不要、16: 16進数、stoulは戻り値がlongなのでキャスト
        value = static_cast<uint16_t>(std::stoul(hex_str, nullptr, 16));
        
        // 変換された数値を std::vector の末尾に追加
        FontData.push_back(value);
    }

    return FontData;
}

/* 一次元配列への二次元的アクセス 
 * Array[行の添え字 * 行数 + 列の添え字]
 * *2バイト配列dataの場合
 *  行はmoji、列はHeight
 * 
 * *1バイト配列pixelの場合
 *  行はHeight、列はWidth
 */
std::string FontReverse(const std::vector<uint16_t>& data,const std::string& name)
{
    uint32_t size = data.size();
    uint32_t allchar = size / FontWidth;        // size/幅(1文字分の要素数)が文字数
    uint16_t word;

    std::vector<uint8_t> pixel(FontWidth*2);    // 配列1行分のメモリを確保
    std::stringstream ss;

    std::string count = std::to_string(allchar);

    ss << "unsigned char "+name+"["+count+"][24] = {\n";

    for (uint32_t moji = 0; moji < allchar; moji++)
    {
        std::fill(pixel.begin(),pixel.end(),0);     // 1行分の処理完了後はバッファをクリア

        for (uint32_t i = 0; i < FontHeight; i++)
        {
            word = data[moji * FontHeight + i];

            for (uint32_t j = 0; j < FontWidth; j++)
            {
                if(word << j & 0x8000)
                {
                    pixel[(i/8) * FontWidth + j] |= 1 << i%8;
                }
            }
            
        }
        ss << "\t{";
        // pixel[0][i]に相当
        ArraytoString(pixel,ss,0);
        // pixel[1][i]に相当。pixel[1][0]はpixel[0][0]からWidthバイト進んだ点になる
        ArraytoString(pixel,ss,FontWidth);

        ss << "},\n";
    }
    ss << "};";

    return ss.str();
}

inline void ArraytoString(const std::vector<uint8_t>& pixel,std::stringstream& ss,uint32_t columnBase)
{
    for (uint32_t i = 0; i < FontWidth; i++)
    {
        // 接頭辞0x、16進数、大文字、桁数2、2桁未満は0埋め、uint8_tは文字扱いなのでintにキャスト
        ss << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)pixel[i + columnBase] << ",";
    }
}
