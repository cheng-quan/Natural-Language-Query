#include "TAChunkTerm.h"

using namespace std;

TAChunkTerm::TAChunkTerm(int iChunkId, string sLabel, string sText, string sStemText, string sPosTag, string sEntityType)
{
	mChunkId=iChunkId;
	mLabel=sLabel;
	mTermTxt=sText;
	mStemTermTxt=sStemText;
	mPosTag=sPosTag;
	mEntityType=sEntityType;
}
TAChunkTerm::TAChunkTerm() {}

// "what", "which", "where", "when", "who"
bool TAChunkTerm::IsWHTerm() const 
{
	if (mPosTag == "WP" || mPosTag == "WP$" || mPosTag == "WRB" || mPosTag == "WDT")
		return true;
	return false;
}

// "can"
bool TAChunkTerm::IsMDTerm() const
{
	if (mPosTag == "MD")
		return true;
	return false;
}

// verb
bool TAChunkTerm::IsVBTerm() const 
{
	if (mPosTag == "VB" || mPosTag == "VBZ" || mPosTag == "VBG" || mPosTag == "VBD" || mPosTag == "VBN" || mPosTag == "VBP")
		return true;
	return false;
}

// operator: "<", ">", "=", "not" 
bool TAChunkTerm::IsOPTerm() const 
{
	if (mTermTxt == "<" || mTermTxt == ">" || mTermTxt == "=" || mTermTxt == "not" || mPosTag == "JJR")
		return true;
	return false;
}

// connection
bool TAChunkTerm::IsCCTerm() const 
{
	if (mPosTag == "CC" || mTermTxt == ",")
		return true;
	return false;
}

// noun
bool TAChunkTerm::IsNPTerm() const 
{
	if (mPosTag=="NN" || mPosTag=="NNS")
		return true;
	return false;
}

// adject
bool TAChunkTerm::IsJJTerm() const 
{
	if (mPosTag=="JJ")
		return true;
	return false;
}

// number
bool TAChunkTerm::IsCDTerm() const 
{
	if (mPosTag=="CD")
		return true;
	return false;
}

// "in", "by"
bool TAChunkTerm::IsINTerm() const 
{
	if (mPosTag=="IN")
		return true;
	return false;	
}

// "the"
bool TAChunkTerm::IsDTTerm() const 
{
	if (mPosTag=="DT")
		return true;
	return false;	
}

// "my"
bool TAChunkTerm::IsPRPSTerm() const 
{
	if (mPosTag=="PRP$")
		return true;
	return false;
}

// "me", "you"
bool TAChunkTerm::IsPRPTerm() const
{
	if (mPosTag=="PRP")
		return true;
	return false;
}

// "top"
bool TAChunkTerm::IsNNTerm() const
{
	if (mPosTag=="NN")
		return true;
	return false;
}

// "best"
bool TAChunkTerm::IsJJSTerm() const
{
	if (mPosTag=="JJS")
		return true;
	return false;
}

// "NY', "DC"
bool TAChunkTerm::IsNNPTerm() const
{
	if (mPosTag=="NNP")
		return true;
	return false;
}

// "'s"
bool TAChunkTerm::IsPOSTerm() const
{
	if (mPosTag == "POS")
		return true;
	return false;
}

// "down"
bool TAChunkTerm::IsRPTerm() const
{
	if (mPosTag == "RP")
		return true;
	return false;
}

// "to"
bool TAChunkTerm::IsTOTerm() const
{
	if (mPosTag == "TO")
		return true;
	return false;
}

// ".", "?"
bool TAChunkTerm::IsPUNCTTerm() const
{
	if (mPosTag == ".")
		return true;
	return false;
}
