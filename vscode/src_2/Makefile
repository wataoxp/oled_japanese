# コンパイラの定義
CXX			:= g++
# すべての警告を有効/デバッグ情報あり/C++14
CXXFLAGS 	:= -Wall -g -std=c++17
# 実行ファイル名
TARGET	 	:= convert

SRCS		:= main.cpp BdftoByte.cpp Regex.cpp FileEdit.cpp
# SRCSの各ファイルを.oとしてリンク
OBJS 		:= $(SRCS:.cpp=.o)

# makeコマンド
$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS)

$(OBJS): $(SRCS)
	$(CXX) $(CXXFLAGS) -c $(SRCS)

# windowsだとデフォでrmコマンドがないのでdel ?Qはプロンプト無しのオプション
clean:
	del /Q $(OBJS)

# makeの流れ
# 1.$(TARGET): $(OBJS) 依存関係のチェック
# 2.$(OBJS): $(SRCS) 依存関係のチェック
# 3.$(CXX) $(CXXFLAGS) -c $(SRCS) オブジェクト生成
# 4.$(CXX) -o $(TARGET) $(OBJS) 実行ファイルの作成
