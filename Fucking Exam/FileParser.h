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
		int question_x_len = 20, question_y_len = 0;
		int part_x_len = 40, part_y_len = 0;
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
		/// <summary>
		/// 
		/// </summary>
		/// <param name="p_buffer">must not null</param>
		/// <param name="bufferSize">must > 0</param>
		/// <param name="p_inVec">must not null</param>
		/// <returns></returns>
		i32 parseFile(char* p_buffer, i32 bufferSize, vector<Block>* p_inVec);
		~FileParser();
	};
}