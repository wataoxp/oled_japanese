#include "BdftoByte.h"

BDF::BDF(/* args */)
{
}

BDF::~BDF()
{
}

void BDF::PushArray(const std::string& line, std::vector<unsigned char>& OutputArray)
{
    std::string toStr;

    if(line.length() % 2 != 0)
    {
        std::cerr << "バイト列が奇数です:" << line << std::endl;
        return;
    }

    // 1行の文字列 (例: "4000") を2文字ずつ処理
    for (size_t i = 0; i < line.length(); i += 2) 
    {
        toStr = line.substr(i, 2);     //i番目から2文字抽出する
        OutputArray.push_back(ChartoHex(toStr));  
    }
}

// std::optional<T> T型の戻り値を基本とし、戻り値にstd::nulloptを使えるようにする
std::optional<std::vector<unsigned char>> BDF::ConvertBDFtoArray(const std::string& filename,const std::string& jiscode)
{
    // 入力ファイル
    std::ifstream ifs(filename);
    // 入力ファイル内の文字列を格納
    std::string Line;
    // 抽出対象のENCODING文字列を準備 (例: "STARTCHAR 2122"を探す)
    std::string TargetString = "STARTCHAR " + jiscode;
    // 状態管理フラグ
    State CurrentState = State::SEARCH_JISCODE;
    // バイト列
    std::vector<unsigned char> ByteData;

    if (!ifs.is_open()) 
    {
        std::cerr << "入力ファイル開封エラー: " << filename << std::endl;
        return std::nullopt;
    }

    // getline()は改行文字\nが出るまでifstreamの内容をstringにコピーする
    // 戻り値は引数のifs。読み込みに成功したときはifsを改行文字の直後を指す状態にする
    while (std::getline(ifs, Line)) 
    {        
        switch (CurrentState)
        {
        case State::SEARCH_JISCODE:
            if(CheckString(Line,TargetString)) CurrentState = State::START_BITMAP;
            break;
        case State::START_BITMAP:
            // startchar_target = "BITMAP";
            if(CheckString(Line,"BITMAP")) CurrentState = State::WAITING_ENDCHAR;
            break;
        case State::WAITING_ENDCHAR:
            if(CheckString(Line,"ENDCHAR")) CurrentState = State::FINISH_CONVERT;
            if(!Line.empty() && CurrentState != State::FINISH_CONVERT) PushArray(Line,ByteData);    // Lineの中身が空かつENDCHARでないとき
            break;
        default:
            break;
        }
    }
    if(CurrentState == State::FINISH_CONVERT)
    {
        return ByteData;
    }
    else
    {
        std::cerr << "該当なし" << std::endl;
        return std::nullopt;
    }
}

std::string BDF::ExportByteArray(const std::vector<unsigned char>& data,const std::string& JisCode)
{
    std::stringstream ss;
    int count = 0;

    ss << "unsigned char " << JisCode << "[] = {" << "\n";

    for (size_t i = 0; i < data.size(); i++)
    {
        if(i % 2 == 0)
        {
            //ss << std::setw(4) << std::setfill(' ');     // \tがでかすぎるので4文字の半角スペース
            ss << '\t';
        }
        // 16進数形式で出力し、0埋め
        ss << "0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)data[i];

        if (i < data.size() - 1)
        {
            ss << ",";
        }
        // 2x回カンマを打った時が改行のタイミング
        count++;
        if(count % 2 == 0)
        {
            ss << "\n";
        }
    }
    ss << "};" << "\n" << "\n";

    return ss.str();
}