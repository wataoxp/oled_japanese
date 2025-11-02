#include "FileEdit.h"

FILEEDIT::FILEEDIT(/* args */)
{
}

FILEEDIT::~FILEEDIT()
{
}

bool FILEEDIT::ArgumentCheck(std::string& Filename)
{
    bool openflag = false;

    while (!openflag)
    {
        std::cout << "BDFファイルを指定してください:";
        std::cin >> Filename;
        std::cout << "BDFファイル名:" << Filename << std::endl;

        if(std::filesystem::is_regular_file(Filename))     // 指定された名前の「ファイル」があるかチェック。ディレクトリはエラー
        {
            std::cout << "BDFファイルが見つかりました" << std::endl;
            openflag = true;
            break;          // 念のため
        }
        else
        {
            std::cerr << "該当するファイルが見つかりません" << std::endl;
            return false;
        }
    }

    return true;
}

bool FILEEDIT::CheckBdfFile(const std::string& Filename,int& x,int& y,int& code)
{
    std::ifstream ifs(Filename);
    std::string Line;
    int result = 0;
    int Height,Width,Firstcode;

    if (!ifs.is_open())
    {
        std::cerr << "BDFファイル開封エラー" << Filename << std::endl;
        return false;
    }

    while (std::getline(ifs,Line))
    {
        // 戻り値がnposでないとき、該当文字列を発見した
        if (Line.find("FONTBOUNDINGBOX") != std::string::npos)
        {
            break;
        }
    }

    result = std::sscanf(Line.c_str(),"FONTBOUNDINGBOX %d %d",&Height,&Width);

    if (result == 2)
    {
        x = Height;
        y = Width;
        std::cout << "Height:" << std::dec << Height << std::endl;
        std::cout << "Width:" << std::dec << Width << std::endl;
    }
    else
    {
        std::cerr << "FontSize Error" << std::endl;
        return false;
    }

    while (std::getline(ifs,Line))
    {
        if (Line.find("STARTCHAR") != std::string::npos)
        {
            break;
        }
    }
    result = std::sscanf(Line.c_str(),"STARTCHAR %x",&Firstcode);
    
    if (result == 1)
    {
        code = Firstcode;
        std::cout << "先頭コード:0x" << std::hex << Firstcode << std::endl; 
    }
    else
    {
        std::cerr << "Code Error" << std::endl;
        return false;
    }
    
    return true;
}

std::string FILEEDIT::InputFileRead(const std::string& filename)
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

//出力ファイルを開く(無ければ新規作成)
std::string FILEEDIT::OutputFileWrite(const std::string& filename,const std::string& After)
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