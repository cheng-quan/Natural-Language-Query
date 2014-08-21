#pragma once
#include "CDSSNLPParser.h"
#include "TextMining/TextAnalysis/TextAnalysis.h"
#include "TextMining/TextAnalysis/PrivateSource/Util.h"

enum MDType
{
	ELEMENT = 0,
	REPORT
};

struct SearchResult{ 
	MDType mdType; //element, report
	std::string name;
	std::string id;
	std::string relatedName;
	std::string relatedID;
	SearchResult(MDType _mdType, std::string _id, std::string _name, std::string _relatedID="", std::string _relatedName="")
	{
		mdType = _mdType;
		name =_name;
		id = _id;
		relatedName = _relatedName;
		relatedID = _relatedID;
	};
	SearchResult() {};
};

struct ObjectInfo {
	EnumDSSObjectType type;
	DSS_ID id;
	CComBSTR name;
	DSS_ID relatedID;
	CComBSTR relatedName;
	ObjectInfo(EnumDSSObjectType iType, DSS_ID iID, BSTR iName, DSS_ID iRelatedID, BSTR iRelatedName)
	{
		type = iType;
		id = iID;
		name = iName;
		relatedID = iRelatedID;
		relatedName = iRelatedName;
	};
	ObjectInfo(EnumDSSObjectType iType, DSS_ID iID, BSTR iName)
	{
		type = iType;
		id = iID;
		name = iName;
		relatedID = iID;
		relatedName = iName;
	};
	ObjectInfo() {};
};

struct SearchConf{
	MDType mdType;
	std::string core;
	std::string queryField;
	std::vector<std::string> returnFields;
};

class MDSearch
{
public:
	MDSearch() {};
	~MDSearch();
	bool Init(const std::string &configPath);
	bool CheckReport(std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int &ei, SearchResult &oMDInfo);
	bool CheckAttribute(CDSSNLPParser *ipNLPParser, std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len, ObjectInfo &oObjectInfo);
	bool CheckMetric(CDSSNLPParser *ipNLPParser, std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len, ObjectInfo &oObjectInfo);
	bool CheckElement(std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len, std::vector<SearchResult> &oElementInfos);
	bool CheckFilter(CDSSNLPParser *ipNLPParser, std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int &ei, ObjectInfo &oObjectInfo);
	bool CheckDiscontinuousFilter(CDSSNLPParser *ipNLPParser, std::vector<TAChunkTerm> &vTerms, std::string index, ObjectInfo &oObjectInfo);
	bool CheckGraph(std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len);
	bool CheckAscend(std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len);
	bool CheckDescend(std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len);
	bool CheckPercent(std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len);
	bool CheckOperation(std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len);
	bool CheckImperative(std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len);
	void SetMDInfo(const std::vector<SearchResult> &ivElementInfos, std::vector<int> &ovTermMDInds, std::vector< std::vector<SearchResult> > &ovvElementInfos, unsigned int si, unsigned int ei);
	void SetMDInfo(const ObjectInfo iObjectInfo , std::vector<int> &ovTermMDInds, std::vector<ObjectInfo> &ovObjectInfos, unsigned int si, unsigned int ei);

	std::map<std::string, EnumDSSGraphType> mmGraphConf;
private:
	bool Query(const std::string url, std::string &oResponse);
	bool Parse(const std::string &jsonDoc, MDType mdType, std::vector<SearchResult> &ovResults);
	void Clear();
	bool DoDetect(MDType mdType, std::vector<TAChunkTerm> &vTerms, SearchResult &ovMDInfo, unsigned int start, unsigned int end);
	bool DoDetect(MDType mdType, std::vector<TAChunkTerm> &vTerms, unsigned int start, unsigned int end, std::vector<SearchResult> &ovElementInfo);
	bool DoDetect(CDSSNLPParser *ipNLPParser, EnumDSSObjectType iObjectType, std::string query, ObjectInfo &oObjectInfo);
	bool IsSameWordNum(std::string query, BSTR result);
	size_t WordNum(std::string query);

	std::map<MDType, SearchConf> mmSearchConf;
	std::map< pair<unsigned int, unsigned int>, bool> AttributeMap;
	std::map< pair<unsigned int, unsigned int>, bool> MetricMap;
	std::map< pair<unsigned int, unsigned int>, bool> ElementMap;
	std::map<std::string, std::string> KeyWordMap;
	CURL *m_curl;
};