#pragma once

#include<Windows.h>
#include"__FuckingExamShared.h"
#include<string>
#include<vector>

using namespace FuckingExam;
using namespace std;

namespace FuckingExam {

	class Block {
	public:
		enum Type {
			None = -1, Part, Question
		};

		Type blockType = Type::None;
		//0-terminated
		u8string blockName;
		u8string content;

		//used by type Question
		int x_len = 20, y_len = 0;
		//used by type Part
		int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
	public:
		Block();
		Block(Type _blockType);
		Block(Type _blockType, u8string _blockName, u8string contentForQuestion);
	};

	class FileParser {
	public:
		u8string errInfo;
	private:
		enum State {
			DetectType, TypePart, TypeQuestion, ReadBlockName, ReadContent, PostBlock
		};
		/// <summary>
		/// 从curPos算起
		/// </summary>
		/// <param name="p_filePath"></param>
		/// <param name=""></param>
		/// <returns></returns>
		i32 getNextLineStartPos(char* p_buffer, i32 bufferSize, i32 curPos);
	public:
		FileParser();
		/// <summary>
		/// 传入一个vector
		/// </summary>
		i32 parseFile(WCHAR* p_filePath, vector<Block>* p_inVec);
		i32 parseFile(char* p_buffer, i32 bufferSize, vector<Block>* p_inVec);
		~FileParser();
	};
}