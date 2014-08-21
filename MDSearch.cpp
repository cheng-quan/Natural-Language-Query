#include "StdAfx.h"
#include "MDSearch.h"
#include "TextMining/TextAnalysis/TextAnalysis.h"

using namespace std;
#define MAX_BUFFER 1000

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

MDSearch::~MDSearch()
{
	Clear();
}

bool MDSearch::Query(const string url, string &oResponse)
{
	string response;
	
	CURLcode res;
	curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &response);
	res = curl_easy_perform(m_curl);
	
	oResponse = response;
	if(res != 0)
		return false;
	return true;
}

bool MDSearch::Parse(const string &jsonDoc, MDType mdType, std::vector<SearchResult> &ovResults)
{
	Json::Reader jsonReader;
	Json::Value root;
	bool b = jsonReader.parse(jsonDoc, root);
	if(!b)
	{
		cout << "Failed to parse json from solr query." << endl;
		return false;
	}
	if (mdType != ELEMENT) {
		const Json::Value response = root.get("response", NULL);
		const Json::Value docs = response.get("docs", NULL);
		for(unsigned int i = 0; i < docs.size(); i ++)
		{
			string id, name, relatedName, relatedID;
			id = docs[i].get(mmSearchConf[mdType].returnFields[0], "").asString();
			name = docs[i].get(mmSearchConf[mdType].returnFields[1], "").asString();
			if(mmSearchConf[mdType].returnFields.size() > 2)
			{
				relatedID = docs[i].get(mmSearchConf[mdType].returnFields[2], "").asString();
				relatedName = docs[i].get(mmSearchConf[mdType].returnFields[3], "").asString();
			}
			
			SearchResult res(mdType, id, name, relatedID, relatedName);
			ovResults.push_back(res);
		}
	} else {
		for (unsigned int i = 0; i < 10 && i < root.size(); i++) {
			string id, name, relatedID, relatedName;
			id = root[i].get(mmSearchConf[mdType].returnFields[0], "").asString();
			name = root[i].get(mmSearchConf[mdType].returnFields[1], "").asString();
			relatedID = root[i].get(mmSearchConf[mdType].returnFields[2], "").asString();
			relatedName = root[i].get(mmSearchConf[mdType].returnFields[3], "").asString();

			SearchResult res(mdType, id, name, relatedID, relatedName);
			ovResults.push_back(res);
		}
	}
	return true;
}

bool MDSearch::Init(const string &configPath)
{
	m_curl = curl_easy_init();
	if (!m_curl)
		return false;

	string searchConfigFile = configPath + "NLPHelper.conf";
	ifstream is(searchConfigFile, ifstream::binary);
	char buffer[MAX_BUFFER];
	is.read(buffer, MAX_BUFFER);
	Json::Reader jsonReader;
	Json::Value root;
	bool b = jsonReader.parse(buffer, root);	
	if (!b) {
		cout << "Failed to parse element search config file!" << endl;
		return false;
	}
	const Json::Value solrCore = root.get("solr", NULL);
	if (solrCore == NULL) {
		cout << "Failed to read solrCore configuration!" << endl;
		return false;
	}
	for (unsigned int i = 0; i < solrCore.size(); i++)
	{
		SearchConf sconf;
		sconf.mdType = static_cast<MDType>(solrCore[i].get("type", 0).asInt());
		sconf.core = solrCore[i].get("core", "").asString();
		sconf.queryField = solrCore[i].get("query_field", "").asString();
		const Json::Value retFields = solrCore[i].get("return_fields", NULL);
		if (retFields == NULL)
		{
			cout << "Failed to read return_fields configuration!" << endl;
			return false;
		}
		for (unsigned int j = 0; j < retFields.size(); j ++)
			sconf.returnFields.push_back( retFields[j].asString());
		mmSearchConf[sconf.mdType] = sconf;
	}
	is.close();
	
	string graphConfigFile = configPath + "GraphType.conf";
	is.open(graphConfigFile, ifstream::binary);
	is.read(buffer, MAX_BUFFER);
	b = jsonReader.parse(buffer, root);
	if (!b) {
		cout << "Failed to parse graph config file!" << endl;
		return false;
	}
	const Json::Value graphCore = root.get("graph", NULL);
	if (solrCore == NULL) {
		cout << "Failed to read graphCore configuration!" << endl;
		return false;
	}
	for (unsigned int i = 0; i < graphCore.size(); i++)
	{
		string name = graphCore[i].get("name", "").asString();
		EnumDSSGraphType type = static_cast<EnumDSSGraphType>(graphCore[i].get("number", "").asInt());
		mmGraphConf[name] = type;
	}
	is.close();

	string keyWordFile = configPath + "KeyWord.conf";
	is.open(keyWordFile, ifstream::binary);
	is.read(buffer, MAX_BUFFER);
	b = jsonReader.parse(buffer, root);
	if (!b) {
		cout << "Failed to parse keyword config file!" << endl;
		return false;
	}
	const Json::Value operationCore = root.get("operation", NULL);
	if (operationCore == NULL) {
		cout << "Failed to read operation configuration!" << endl;
		return false;
	}
	for (unsigned int i = 0; i < operationCore.size(); i++)
	{
		string name = operationCore[i].get("name", "").asString();
		KeyWordMap[name] = "operation";
	}
	const Json::Value imperativeCore = root.get("imperative", NULL);
	if (imperativeCore == NULL) {
		cout << "Failed to read imperative configuration!" << endl;
		return false;
	}
	for (unsigned int i = 0; i < imperativeCore.size(); i++)
	{
		string name = imperativeCore[i].get("name", "").asString();
		KeyWordMap[name] = "imperative";
	}
	const Json::Value percentCore = root.get("percent", NULL);
	if (percentCore == NULL) {
		cout << "Failed to read percent configuration!" << endl;
		return false;
	}
	for (unsigned int i = 0; i < percentCore.size(); i++)
	{
		string name = percentCore[i].get("name", "").asString();
		KeyWordMap[name] = "percent";
	}
	const Json::Value ascendCore = root.get("ascend", NULL);
	if (ascendCore == NULL) {
		cout << "Failed to read ascend configuration!" << endl;
		return false;
	}
	for (unsigned int i = 0; i < ascendCore.size(); i++)
	{
		string name = ascendCore[i].get("name", "").asString();
		KeyWordMap[name] = "ascend";
	}
	const Json::Value descendCore = root.get("descend", NULL);
	if (descendCore == NULL) {
		cout << "Failed to read descend configuration!" << endl;
		return false;
	}
	for (unsigned int i = 0; i < descendCore.size(); i++)
	{
		string name = descendCore[i].get("name", "").asString();
		KeyWordMap[name] = "descend";
	}
	is.close();

	return true;
}

void MDSearch::Clear()
{
	if(m_curl)
		curl_easy_cleanup(m_curl);
}

bool MDSearch::CheckReport( std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int &ei , SearchResult &oMDInfo)
{
	ei = (unsigned int) vTerms.size();
	//if(DoDetect(REPORT, vTerms, oMDInfo, si, ei))
	//	return true;
	return false;
}

bool MDSearch::DoDetect( MDType mdType, std::vector<TAChunkTerm> &vTerms, SearchResult &ovMDInfo, unsigned int start, unsigned int end )
{
	string qstr;
	for(unsigned int i = start; i < end; i ++)
	{
		qstr += vTerms[i].mTermTxt + "%20";
	}
	qstr = qstr.substr(0, qstr.length()-3);
	//phrase search
	qstr = "\"" + qstr + "\"";
	vector<SearchResult> result;
	string response;
	string url;
	url = mmSearchConf[mdType].core + "/select?q=" + mmSearchConf[mdType].queryField + "%3A" + qstr + "&wt=json";
	bool bSucceful = Query(url, response);
	if(bSucceful && response.length() > 0)
	{
		Parse(response, mdType, result);
	}
	for (size_t i = 0; i < result.size(); i++)
	{
		if (WordNum(result[i].name) == end - start) {
			ovMDInfo = result[i];
			return true;
		}
	}

	return false;
}

bool MDSearch::DoDetect(MDType mdType, std::vector<TAChunkTerm> &vTerms, unsigned int start, unsigned int end, std::vector<SearchResult> &ovElementInfos)
{
	bool found = false;
	string qstr;
	for(unsigned int i = start; i < end; i ++)
	{
		qstr += vTerms[i].mTermTxt + "%20";
	}
	qstr = qstr.substr(0, qstr.length()-3);
	//phrase search
	qstr = "\"" + qstr + "\"";
	vector<SearchResult> result;
	string response;
	string url;
	url = mmSearchConf[mdType].core + "/search?field=" + mmSearchConf[mdType].queryField + "&query=" + qstr;
	bool bSucceful = Query(url, response);
	if(bSucceful && response.length() > 0)
	{
		Parse(response, mdType, result);
	}
	ovElementInfos.clear();
	for (unsigned int i = 0; i < result.size(); i++)
	{
		if (WordNum(result[i].name) == end - start) {
			ovElementInfos.push_back(result[i]);
			found = true;
		}
	}

	return found;
}

bool MDSearch::DoDetect(CDSSNLPParser *ipNLPParser, EnumDSSObjectType iObjectType, string query, ObjectInfo &oObjectInfo)
{
	query = "\"" + query + "\"";
	CComBSTR lObjectName(MBase::UTF8ToWideChar(query.c_str()).c_str());

	vector<EnumDSSObjectType> iObjectTypes;
	iObjectTypes.push_back(iObjectType);
	CComPtr<ICDSSFolder> licpResultFolder;

	HRESULT hr = ipNLPParser->hMDSearch(lObjectName, iObjectTypes, &licpResultFolder);
	if (FAILED(hr)) 
		return false;

	Int32 iCount = 0;
	hr = licpResultFolder->Count(&iCount);
	if (FAILED(hr) || iCount <= 0)
		return false;

	for (Int32 i = 1; i <= iCount; i++) {
		CComPtr<ICDSSObjectInfo> licpObjInfo;
		hr = licpResultFolder->Item(i, &licpObjInfo);
		if (FAILED(hr))
			return false;

		CComBSTR lName;
		hr = licpObjInfo->get_NameNS(&lName);
		if (FAILED(hr)) 
			return false;
		if (IsSameWordNum(query, lName))
		{				
			hr = licpObjInfo->get_ID(&(oObjectInfo.id));
			if (FAILED(hr)) 
				return false;

			oObjectInfo.name = lName;

			return true;
		}
	}
	
	return false;
}

bool MDSearch::CheckAttribute(CDSSNLPParser *ipNLPParser, std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len, ObjectInfo &oObjectInfo)
{
	if (si + len > vTerms.size())
		return false;
	if (AttributeMap.find(make_pair(si, len)) != AttributeMap.end() && AttributeMap[make_pair(si, len)] == false)
		return false;
	bool match = false;
	if (len == 1) {
		if (vTerms[si].IsNPTerm() || vTerms[si].IsJJTerm() || vTerms[si].IsVBTerm())
			match = true;
	} else if (len == 2) {
		if (vTerms[si].IsNPTerm() && (vTerms[si+1].IsVBTerm() || vTerms[si+1].IsNPTerm()))
			match = true;
	} else if (len == 3) {
		if (vTerms[si].IsNPTerm() && vTerms[si+1].IsNPTerm() && vTerms[si+2].IsNPTerm())
			match = true;
	} else if (len == 4) {
		if (vTerms[si].IsNPTerm() && vTerms[si+1].IsNPTerm() && vTerms[si+2].IsNPTerm() && vTerms[si+3].IsNPTerm())
			match = true;
	}
	bool found = false;
	if (match)
	{
		string query;
		for (unsigned int i = si; i < si + len; i++)
		{
			query += vTerms[i].mTermTxt + " ";
		}
		query = query.substr(0, query.length()-1);
		if (DoDetect(ipNLPParser, DssTypeAttribute, query, oObjectInfo)) 
			found = true;
	}
	AttributeMap[make_pair(si, len)] = found;
	return found;
}

bool MDSearch::CheckMetric(CDSSNLPParser *ipNLPParser, std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len, ObjectInfo &oObjectInfo)
{
	if (si + len > vTerms.size())
		return false;
	if (MetricMap.find(make_pair(si, len)) != MetricMap.end() && MetricMap[make_pair(si, len)] == false) 
		return false;
	bool match = false;
	if (len == 1) {
		if (vTerms[si].IsNPTerm())
			match = true;
	} else if (len == 2) {
		if (vTerms[si].IsNPTerm() && (vTerms[si+1].IsVBTerm() || vTerms[si+1].IsNPTerm()))
			match = true;
	} else if (len == 3) {
		if (vTerms[si].IsNPTerm() && vTerms[si+1].IsINTerm() && vTerms[si+2].IsNPTerm())
			match = true;
	} else if (len == 4) {
		if ((vTerms[si].IsNPTerm() || vTerms[si].IsJJTerm()) && vTerms[si+1].IsNPTerm() && (vTerms[si+2].IsNPTerm() || vTerms[si+2].IsINTerm()) && vTerms[si+3].IsNPTerm())
			match = true;
	}
	bool found = false;
	if (match)
	{
		string query;
		for (unsigned int i = si; i < si + len; i++)
		{
			query += vTerms[i].mTermTxt + " ";
		}
		query = query.substr(0, query.length()-1);
		if (DoDetect(ipNLPParser, DssTypeMetric, query, oObjectInfo))
			found = true;
	}
	MetricMap[make_pair(si, len)] = found;
	return found;
}

bool MDSearch::CheckElement(std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len, std::vector<SearchResult> &ovElementInfos)
{
	if (si + len > vTerms.size())
		return false;
	if (ElementMap.find(make_pair(si, len)) != ElementMap.end() && ElementMap[make_pair(si, len)] == false) 
		return false;
	bool match = false;
	if (len == 1) {
		if (vTerms[si].IsCDTerm() || vTerms[si].IsNNPTerm() || vTerms[si].IsNPTerm())
			match = true;
	} else if (len == 2) {
		if (vTerms[si].IsNNPTerm() && vTerms[si+1].IsNNPTerm())
			match = true;
	} else if (len == 3) {
		if (vTerms[si].IsNNPTerm() && vTerms[si+1].IsNNPTerm() && vTerms[si+2].IsNNPTerm())
			match = true;
	} else if (len == 4) {
		if (vTerms[si].IsNNPTerm() && vTerms[si+1].IsNNPTerm() && vTerms[si+2].IsNNPTerm() && vTerms[si+3].IsNNPTerm())
			match = true;
	}
	bool found = false;
	if(match)
	{
		if (DoDetect(ELEMENT, vTerms, si, si + len, ovElementInfos))
			found = true;
	}
	ElementMap[make_pair(si, len)] = found;
	return found;
}

bool MDSearch::CheckFilter(CDSSNLPParser *ipNLPParser, std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int &ei, ObjectInfo &oObjectInfo)
{
	ei = -1;
	if (vTerms[si].IsDTTerm() || vTerms[si].IsPRPSTerm() || vTerms[si].IsJJTerm()) {
		if (si+1 < vTerms.size() && vTerms[si+1].IsNPTerm()) {
			ei = si+2;
			while (ei < vTerms.size() && vTerms[ei].IsNPTerm())
				ei++;
		}
	}
	if (ei != -1) {
		string query;
		for (unsigned int i = si; i < ei; i++)
			query += vTerms[i].mTermTxt + " ";
		query = query.substr(0, query.length()-1);
		if (DoDetect(ipNLPParser, DssTypeFilter, query, oObjectInfo))
			return true;
	}

	return false;
}

bool MDSearch::CheckDiscontinuousFilter(CDSSNLPParser *ipNLPParser, std::vector<TAChunkTerm> &vTerms, string index, ObjectInfo &oObjectInfo)
{
	string query;
	for (int i = 0; i < index.size() / 2; i++) {
		int num = atoi(index.substr(2*i, 2).c_str());
		query += vTerms[num].mTermTxt + " ";
	}
	query = query.substr(0, query.length()-1);

	if (DoDetect(ipNLPParser, DssTypeFilter, query, oObjectInfo))
		return true;

	return false;
}

bool MDSearch::CheckGraph(std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len)
{
	if (si + len > vTerms.size())
		return false;
	bool match = false;
	if (len == 1) {
		if (vTerms[si].IsNPTerm() || vTerms[si].IsVBTerm())
			match = true;
	} else if (len == 2) {
		if (vTerms[si].IsNPTerm() && vTerms[si+1].IsNPTerm())
			match = true;
	}
	if (match) {
		string query;
		for (unsigned int i = si; i < si + len; i++)
			query += vTerms[i].mTermTxt + " ";
		query = query.substr(0, query.length()-1);
		query = tolower(query);
		if (mmGraphConf.find(query) != mmGraphConf.end()) 
			return true;
	}

	return false;
}

bool MDSearch::CheckAscend(std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len)
{
	if (si + len > vTerms.size())
		return false;
	bool match = false;
	if (len == 1) {
		if (vTerms[si].IsNNTerm() || vTerms[si].IsJJTerm() || vTerms[si].IsJJSTerm())
			match = true;
	} 
	if (match)
	{
		string query;
		for (unsigned int i = si; i < si + len; i++)
		{
			query += vTerms[i].mTermTxt + " ";
		}
		query = query.substr(0, query.length()-1);
		query = tolower(query);
		if (KeyWordMap.find(query) != KeyWordMap.end() && KeyWordMap[query] == "ascend")
			return true;
	}
	return false;
}

bool MDSearch::CheckDescend(std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len)
{
	if (si + len > vTerms.size())
		return false;
	bool match = false;
	if (len == 1) {
		if (vTerms[si].IsNNTerm() || vTerms[si].IsJJTerm() || vTerms[si].IsJJSTerm())
			match = true;
	} 
	if (match)
	{
		string query;
		for (unsigned int i = si; i < si + len; i++)
		{
			query += vTerms[i].mTermTxt + " ";
		}
		query = query.substr(0, query.length()-1);
		query = tolower(query);
		if (KeyWordMap.find(query) != KeyWordMap.end() && KeyWordMap[query] == "descend")
			return true;
	}
	return false;
}

bool MDSearch::CheckPercent(std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len)
{
	if (si + len > vTerms.size())
		return false;
	bool match = false;
	if (len == 1) {
		if (vTerms[si].IsNNTerm())
			match = true;
	} 
	if (match)
	{
		string query;
		for (unsigned int i = si; i < si + len; i++)
		{
			query += vTerms[i].mTermTxt + " ";
		}
		query = query.substr(0, query.length()-1);
		query = tolower(query);
		if (KeyWordMap.find(query) != KeyWordMap.end() && KeyWordMap[query] == "percent")
			return true;
	}
	return false;
}

bool MDSearch::CheckOperation(std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len)
{
	if (si + len > vTerms.size())
		return false;
	bool match = false;
	if (len == 1) {
		if (vTerms[si].IsVBTerm())
			match = true;
	} 
	if (match)
	{
		string query;
		for (unsigned int i = si; i < si + len; i++)
		{
			query += vTerms[i].mTermTxt + " ";
		}
		query = query.substr(0, query.length()-1);
		query = tolower(query);
		if (KeyWordMap.find(query) != KeyWordMap.end() && KeyWordMap[query] == "operation")
			return true;
	}
	return false;
}

bool MDSearch::CheckImperative(std::vector<TAChunkTerm> &vTerms, unsigned int si, unsigned int len)
{
	if (si + len > vTerms.size())
		return false;
	bool match = false;
	if (len == 1) {
		if (vTerms[si].IsVBTerm() || vTerms[si].IsNNTerm())
			match = true;
	} 
	if (match)
	{
		string query;
		for (unsigned int i = si; i < si + len; i++)
		{
			query += vTerms[i].mTermTxt + " ";
		}
		query = query.substr(0, query.length()-1);
		query = tolower(query);
		if (KeyWordMap.find(query) != KeyWordMap.end() && KeyWordMap[query] == "imperative")
			return true;
	}
	return false;
}

void MDSearch::SetMDInfo(const std::vector<SearchResult> &ivElementInfos, std::vector<int> &ovTermMDInds, std::vector< std::vector<SearchResult> > &ovvElementInfos, unsigned int si, unsigned int ei)
{
	ovvElementInfos.push_back(ivElementInfos);
	for(unsigned int i = si; i < ei; i++)
		ovTermMDInds[i] = (unsigned int) ovvElementInfos.size() - 1;
}

void MDSearch::SetMDInfo(const ObjectInfo iObjectInfo , std::vector<int> &ovTermMDInds, std::vector<ObjectInfo> &ovObjectInfos, unsigned int si, unsigned int ei)
{
	ovObjectInfos.push_back(iObjectInfo);
	for(unsigned int i = si; i < ei; i++)
		ovTermMDInds[i] = (unsigned int) ovObjectInfos.size() - 1;
}

bool MDSearch::IsSameWordNum(std::string query, BSTR result)
{
	size_t queryWordNum = WordNum(query);
	wstring wstrResult(result, SysStringLen(result));
	string strResult = MBase::WideCharToUTF8(wstrResult.c_str()).c_str();
	size_t resultWordNum = WordNum(strResult);
	if (queryWordNum == resultWordNum)
		return true;
	else
		return false;
}

size_t MDSearch::WordNum(std::string query)
{
	size_t found = 0;
	size_t wordNum = 0;
	while (found != string::npos) {
		if (found == 0)
			found = query.find(" ", 0);
		else
			found = query.find(" ", found + 1);
		wordNum++;
	}
	return wordNum;
}
