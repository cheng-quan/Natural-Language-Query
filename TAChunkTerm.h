/*
** Copyright 2014, MicroStrategy.Inc
** All rights reserved.
**
**   File Name: TAChunkTerm.h 
** Description: record term information in Salience Chunk 
**      Author: Cheng Quan
**     Created: 2014-02-28
**    Modifier: Cheng Quan
**    Modified: 2014-02-28
*/

#ifndef __TAChunkTerm_h
#define __TAChunkTerm_h

#include <String>

class TAChunkTerm {
public:
	TAChunkTerm(int iChunkId, std::string sLabel, std::string sText, std::string sStemText, std::string sPosTag, std::string sEntityType);
	TAChunkTerm();
	bool IsWHTerm() const;	// "what", "which", "where", "when", "who"
	bool IsMDTerm() const;	// "can"
	bool IsVBTerm() const;	// verb
	bool IsOPTerm() const;	// operator: "<", ">", "=", "not" 
	bool IsCCTerm() const;	// connection
	bool IsNPTerm() const;	// noun
	bool IsJJTerm() const;	// adject
	bool IsCDTerm() const;	// number
	bool IsINTerm() const;	// "in", "by"
	bool IsDTTerm() const;	// "the"
	bool IsPRPSTerm() const;	// "my"
	bool IsPRPTerm() const;	// "me", "you"
	bool IsNNTerm() const;	// "top"
	bool IsJJSTerm() const;	// "best"
	bool IsNNPTerm() const;	// "NY', "DC"
	bool IsPOSTerm() const;	// "'s"
	bool IsRPTerm() const;	// "down"
	bool IsTOTerm() const;	// "to"
	bool IsPUNCTTerm() const;	// ".', "?"

	int mChunkId;
	std::string mLabel;
	std::string mTermTxt;
	std::string mStemTermTxt;
	std::string mPosTag;
	std::string mEntityType;
};

#endif