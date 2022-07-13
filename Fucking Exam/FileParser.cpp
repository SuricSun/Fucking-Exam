#include "FileParser.h"

FuckingExam::FileParser::FileParser() {
}

i32 FuckingExam::FileParser::getNextLineStartPos(char* p_buffer, i32 bufferSize, i32 curPos) {

	//从curPos位置读取直到读取到换行符
	i32 tmpCurPos = curPos;
	while (true) {
		if (tmpCurPos >= bufferSize) {
			//返回
			return tmpCurPos;
		}

		if (p_buffer[tmpCurPos] == '\r' || p_buffer[tmpCurPos] == '\n') {
			//循环把后面的换行都读取出来
			while (true) {
				if (tmpCurPos >= bufferSize) {
					//返回
					return tmpCurPos;
				}

				if (p_buffer[tmpCurPos] != '\r' && p_buffer[tmpCurPos] != '\n') {
					return tmpCurPos;
				}

				tmpCurPos++;
			}
		}

		tmpCurPos++;
	}
}

i32 FuckingExam::FileParser::parseFile(char* p_buffer, i32 bufferSize, vector<Block>* p_inVec) {

	//清空vec
	p_inVec->clear();

	//设置state
	i32 state0 = 0;
	i32 state1 = 0;
	i32 curPos = 0;
	i32 markPos = 0;
	Block tmpBlock;

	//buffer太小
	if (bufferSize < 1) {
		this->errInfo = u8"文本为空!";
		return -1;
	}

	//初始化状态
	if (p_buffer[curPos] == '@') {
		state0 = State::TypePart;
		state1 = State::ReadBlockName;
		tmpBlock.blockType = Block::Part;
	} else if (p_buffer[curPos] == '#') {
		state0 = State::TypeQuestion;
		state1 = State::ReadBlockName;
		tmpBlock.blockType = Block::Question;
	} else {
		this->errInfo = u8"文本开头字符为非控制字符!";
		return -1;
	}

	//步进1格
	curPos++;

	while (true) {

		if (state0 == State::DetectType) {
			if (curPos >= bufferSize) {
				//DetectType状态读取完了，程序退出
				return 0;
			}
			if (p_buffer[curPos] == '@') {
				state0 = State::TypePart;
				state1 = State::ReadBlockName;
				tmpBlock.blockType = Block::Part;
			} else if (p_buffer[curPos] == '#') {
				state0 = State::TypeQuestion;
				state1 = State::ReadBlockName;
				tmpBlock.blockType = Block::Question;
			}
			//步进1格
			curPos++;
		}

		if (state1 == State::ReadBlockName) {
			//读取一行
			if (curPos >= bufferSize) {
				//更新状态
				state1 = State::PostBlock;
				continue;
			}
			markPos = curPos;
			curPos = this->getNextLineStartPos(p_buffer, bufferSize, curPos);
			//复制到block
			tmpBlock.blockName.assign(p_buffer + markPos, p_buffer + curPos);
			//更新状态
			state1 = State::ReadContent;
		} else if (state1 == State::ReadContent) {
			//循环读取
			while (true) {
				if (curPos >= bufferSize) {
					//更新状态
					state1 = State::PostBlock;
					break;
				}
				//检查当前行头是否为控制字符, 是：更新状态为PostBlock 否：把当前行读取并复制到block
				if (p_buffer[curPos] == '@' || p_buffer[curPos] == '#') {
					state1 = State::PostBlock;
					break;
				}
				//复制
				markPos = curPos;
				curPos = this->getNextLineStartPos(p_buffer, bufferSize, curPos);
				//复制到block
				tmpBlock.content.append(p_buffer + markPos, p_buffer + curPos);
			}
		} else if (state1 == State::PostBlock) {
			//ez
			state0 = State::DetectType;
			p_inVec->emplace_back(tmpBlock);
			//初始化新的block
			tmpBlock = Block();
		}
	}

	return 0;
}

i32 FuckingExam::FileParser::parseFile(WCHAR* p_filePath, vector<Block>* p_inVec) {

	//Open file
	HANDLE hFile = CreateFile(p_filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		this->errInfo = u8"无法打开文件: hFile == INVALID_HANDLE_VALUE";
		return -1;
	}
	//get size
	LARGE_INTEGER li = {};
	if (GetFileSizeEx(hFile, &li) == false) {
		this->errInfo = u8"无法获取文件大小: GetFileSizeEx(hFile, &li) == false";
		return -1;
	}
	i64 fileSize = li.QuadPart;
	//read to buffer
	char* p_buffer = new char[fileSize];
	i64 bytesRead = 0;
	if (ReadFile(hFile, p_buffer, fileSize, (DWORD*)&bytesRead, nullptr) == false) {
		this->errInfo = u8"读取文件失败: ReadFile(hFile, p_buffer, fileSize, (DWORD*)&bytesRead, nullptr) == false";
		return -1;
	}
	if (fileSize != bytesRead) {
		this->errInfo = u8"读取文件失败: fileSize != bytesRead";
		return -1;
	}
	//关闭文件
	if (CloseHandle(hFile) == 0) {
		this->errInfo = u8"关闭文件失败: CloseHandle(hFile) == 0";
		return -1;
	}

	return this->parseFile(p_buffer, fileSize, p_inVec);
}

FuckingExam::FileParser::~FileParser() {
}

FuckingExam::Block::Block() {

	this->blockType = Type::None;
}

FuckingExam::Block::Block(Type _blockType) {

	this->blockType = _blockType;
}

FuckingExam::Block::Block(Type _blockType, u8string _blockName, u8string content) {

	this->blockType = _blockType;
	this->blockName = _blockName;
	this->content = content;
}
