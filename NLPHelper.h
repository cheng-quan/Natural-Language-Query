#ifndef NLPHelper_h
#define NLPHelper_h 1

#include <string>
#include "Base/Base/GUID.h"

#ifdef WIN32
#ifdef NLPHELPER_DLL_EXPORT
#define	NLPHELPER_DLL_EXIM	_declspec(dllexport)
#else
#define	NLPHELPER_DLL_EXIM	_declspec(dllimport)
#endif
#else
#define	NLPHELPER_DLL_EXIM
#endif //WIN32

NLPHELPER_DLL_EXIM bool string2Guid(::GUID& orGUID, const std::string &s);
NLPHELPER_DLL_EXIM int testAntlrCMD(const std::string iFileName, const std::string oFileName);
NLPHELPER_DLL_EXIM void writeChunkTerm(const std::string iFileName, const std::string oFileName);
NLPHELPER_DLL_EXIM void compareOutput(const std::string iCompareFile, const std::string iOutputFile);

#endif