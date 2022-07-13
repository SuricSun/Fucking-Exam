#include "FileParser.h"

FuckingExam::FileParser::FileParser() {
}

i32 FuckingExam::FileParser::getNextLineStartPos(char* p_buffer, i32 bufferSize, i32 curPos) {

	//��curPosλ�ö�ȡֱ����ȡ�����з�
	i32 tmpCurPos = curPos;
	while (true) {
		if (tmpCurPos >= bufferSize) {
			//����
			return tmpCurPos;
		}

		if (p_buffer[tmpCurPos] == '\r' || p_buffer[tmpCurPos] == '\n') {
			//ѭ���Ѻ���Ļ��ж���ȡ����
			while (true) {
				if (tmpCurPos >= bufferSize) {
					//����
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

	//���vec
	p_inVec->clear();

	//����state
	i32 state0 = 0;
	i32 state1 = 0;
	i32 curPos = 0;
	i32 markPos = 0;
	Block tmpBlock;

	//buffer̫С
	if (bufferSize < 1) {
		this->errInfo = u8"�ı�Ϊ��!";
		return -1;
	}

	//��ʼ��״̬
	if (p_buffer[curPos] == '@') {
		state0 = State::TypePart;
		state1 = State::ReadBlockName;
		tmpBlock.blockType = Block::Part;
	} else if (p_buffer[curPos] == '#') {
		state0 = State::TypeQuestion;
		state1 = State::ReadBlockName;
		tmpBlock.blockType = Block::Question;
	} else {
		this->errInfo = u8"�ı���ͷ�ַ�Ϊ�ǿ����ַ�!";
		return -1;
	}

	//����1��
	curPos++;

	while (true) {

		if (state0 == State::DetectType) {
			if (curPos >= bufferSize) {
				//DetectType״̬��ȡ���ˣ������˳�
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
			//����1��
			curPos++;
		}

		if (state1 == State::ReadBlockName) {
			//��ȡһ��
			if (curPos >= bufferSize) {
				//����״̬
				state1 = State::PostBlock;
				continue;
			}
			markPos = curPos;
			curPos = this->getNextLineStartPos(p_buffer, bufferSize, curPos);
			//���Ƶ�block
			tmpBlock.blockName.assign(p_buffer + markPos, p_buffer + curPos);
			//����״̬
			state1 = State::ReadContent;
		} else if (state1 == State::ReadContent) {
			//ѭ����ȡ
			while (true) {
				if (curPos >= bufferSize) {
					//����״̬
					state1 = State::PostBlock;
					break;
				}
				//��鵱ǰ��ͷ�Ƿ�Ϊ�����ַ�, �ǣ�����״̬ΪPostBlock �񣺰ѵ�ǰ�ж�ȡ�����Ƶ�block
				if (p_buffer[curPos] == '@' || p_buffer[curPos] == '#') {
					state1 = State::PostBlock;
					break;
				}
				//����
				markPos = curPos;
				curPos = this->getNextLineStartPos(p_buffer, bufferSize, curPos);
				//���Ƶ�block
				tmpBlock.content.append(p_buffer + markPos, p_buffer + curPos);
			}
		} else if (state1 == State::PostBlock) {
			//ez
			state0 = State::DetectType;
			p_inVec->emplace_back(tmpBlock);
			//��ʼ���µ�block
			tmpBlock = Block();
		}
	}

	return 0;
}

i32 FuckingExam::FileParser::parseFile(WCHAR* p_filePath, vector<Block>* p_inVec) {

	//Open file
	HANDLE hFile = CreateFile(p_filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		this->errInfo = u8"�޷����ļ�: hFile == INVALID_HANDLE_VALUE";
		return -1;
	}
	//get size
	LARGE_INTEGER li = {};
	if (GetFileSizeEx(hFile, &li) == false) {
		this->errInfo = u8"�޷���ȡ�ļ���С: GetFileSizeEx(hFile, &li) == false";
		return -1;
	}
	i64 fileSize = li.QuadPart;
	//read to buffer
	char* p_buffer = new char[fileSize];
	i64 bytesRead = 0;
	if (ReadFile(hFile, p_buffer, fileSize, (DWORD*)&bytesRead, nullptr) == false) {
		this->errInfo = u8"��ȡ�ļ�ʧ��: ReadFile(hFile, p_buffer, fileSize, (DWORD*)&bytesRead, nullptr) == false";
		return -1;
	}
	if (fileSize != bytesRead) {
		this->errInfo = u8"��ȡ�ļ�ʧ��: fileSize != bytesRead";
		return -1;
	}
	//�ر��ļ�
	if (CloseHandle(hFile) == 0) {
		this->errInfo = u8"�ر��ļ�ʧ��: CloseHandle(hFile) == 0";
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
