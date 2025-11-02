#include "Regex.h"

REGEX::REGEX(/* args */)
{
}

REGEX::~REGEX()
{
}
void REGEX::SetFormat(int height,int width)
{
    font.Height = height;
    font.Width = width;
}

std::string REGEX::EditFileString(const std::string& before,int width)
{
    std::string str = before;   //置換前の文字列
    std::regex pattern;         //置換対象の正規表現を記述
    std::string replacement;    //置換する文字列
    std::string After;          //置換後の文字列        
    std::string ArrayName;

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

    After.pop_back();       // 末尾の改行を消す

    // 文字列内に挿入できるのは文字列だけ
    if (width > 8)
    {
        ArrayName = "unsigned short shinonome[]["+std::to_string(width)+"] = {\n";
    }
    else
    {
        ArrayName = "unsigned char shinonome[]["+std::to_string(width)+"] = {\n";
    }
    
    After.insert(0,ArrayName);
    After.append("};");

    return After;
}

//文字列を置換する
std::string REGEX::RegexReplace(const std::string& before,const std::regex& pattern,const std::string& replacement)
{
    std::string content = before;
    content = std::regex_replace(content,pattern,replacement);      //置換後の文字列を格納

    return content;
}

std::vector<uint16_t> REGEX::StringtoHex(const std::string& file)
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
    std::string HexStr;
    // hex_strを16進数として格納
    uint16_t value;

    // itが文字列の終点を指したとき、itはendと同じ値を指す
    for (; it != end; it++)
    {
        // (*it)[0].str() はマッチした文字列全体（例: "0xABCD"）
        HexStr = (*it)[0].str();

        // 文字列（テキスト）を16進数として読み込み、uint16_t型の数値に変換
        // nullptr: エラーチェック不要、16: 16進数、stoulは戻り値がlongなのでキャスト
        value = static_cast<uint16_t>(std::stoul(HexStr, nullptr, 16));
        
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
std::string REGEX::FontReverse(const std::vector<uint16_t>& data,const std::string& name)
{
    uint32_t size = data.size();
    uint32_t allchar = size / font.Width;        // size/幅(1文字分の要素数)が文字数
    uint16_t word;
    uint32_t Bits = 0x80;

    std::vector<uint8_t> pixel(font.Width*2);    // 配列1行分のメモリを確保
    std::stringstream ss;

    std::string count = std::to_string(allchar);

    if (font.Width > 8)
    {
        Bits = 0x8000;
    }
    

    ss << "unsigned char "+name+"["+count+"][24] = {\n";

    for (uint32_t moji = 0; moji < allchar; moji++)
    {
        std::fill(pixel.begin(),pixel.end(),0);     // 1行分の処理完了後はバッファをクリア

        for (int i = 0; i < font.Height; i++)
        {
            word = data[moji * font.Height + i];

            for (int j = 0; j < font.Width; j++)
            {
                if(word << j & Bits)
                {
                    pixel[(i/8) * font.Width + j] |= 1 << i%8;
                }
            }
            
        }
        ss << "\t{";
        // pixel[0][i]に相当
        ArraytoString(pixel,ss,0);
        // pixel[1][i]に相当。pixel[1][0]はpixel[0][0]からWidthバイト進んだ点になる
        ArraytoString(pixel,ss,font.Width);

        ss << "},\n";
    }
    ss << "};";

    return ss.str();
}
/*** 反転した数値を文字列に格納 ***/
void REGEX::ArraytoString(const std::vector<uint8_t>& pixel,std::stringstream& ss,uint32_t columnBase)
{
    for (int i = 0; i < font.Width; i++)
    {
        // 接頭辞0x、16進数、大文字、桁数2、2桁未満は0埋め、uint8_tは文字扱いなのでintにキャスト
        ss << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)pixel[i + columnBase] << ",";
    }
}
