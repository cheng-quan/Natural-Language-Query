grammar CMD;

options {language = Cpp;} 
 
@parser::header
{
	#include "StdAfx.h"
	#include "TextMining/TextAnalysis/TextAnalysis.h"
	#include "CMDLexer.hpp"
	#include "MDSearch.h"
	#include "NLPHelper.h"
	#include "CDSSNLPParser.h"
 
	using namespace std;
}

@lexer::header
{
	#include "StdAfx.h"
}

@lexer::namespace { User }
@parser::namespace{ User }

@lexer::traits 
{
	class CMDLexer; 
	class CMDParser; 

 	template<class ImplTraits>
 	class UserTraits : public antlr3::CustomTraitsBase<ImplTraits>
 	{
 		public:
		//for using the token stream which deleted the tokens, once it is reduced to a rule
		//but it leaves the start and stop tokens. So they can be accessed as usual
  		static const bool TOKENS_ACCESSED_FROM_OWNING_RULE = true;
 	};

 	typedef antlr3::Traits< CMDLexer, CMDParser, UserTraits > CMDLexerTraits;
 	typedef CMDLexerTraits CMDParserTraits;
}

cmd[const vector<TAChunkTerm>& ivTerms, MDSearch *ipMDSearch, bool iflag, bool iClearFlag, ofstream *ipOutput, CDSSNLPParser *ipNLPParser]
scope
{
	vector<TAChunkTerm> mvTerms;
	vector<SearchResult> mvMDInfos;
	vector<SearchResult> mvElementInfos;
	vector< vector<SearchResult> > mvvElementInfos;
	vector<ObjectInfo> mvObjectInfos;
 	vector<int> mvTermMDInds;
 	MDSearch *mpMDSearch;
 	bool mflag;
 	bool mClearFlag;
 	ofstream *mpOutput;
 	CDSSNLPParser *mpNLPParser;
 	CComPtr<ICDSSNode> mpiNode;
 	CComPtr<ICDSSNode> mpoNode;
 	unsigned int msi;
 	unsigned int mei;
 	string relatedAttribute;
 	SearchResult mSearchResult;
 	ObjectInfo mObjectInfo;
 	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
}
@init
{
 	$cmd::mvTerms=ivTerms;
 	$cmd::mpMDSearch = ipMDSearch;
 	$cmd::mflag = iflag;
 	$cmd::mClearFlag = iClearFlag;
 	$cmd::mpOutput = ipOutput;
 	$cmd::mpNLPParser = ipNLPParser;
 	$cmd::mpiNode = NULL;
 	$cmd::mpoNode = NULL;
 	
 	for(unsigned int i = 0; i < $cmd::mvTerms.size(); i++)
  		$cmd::mvTermMDInds.push_back(-1);
 	$cmd::msi = 0;
 	$cmd::mei = 0;
}
: request (exist_report | (cc? object)+) punct? EOF;
 
request // "where is", "what's", "give me the", "can you", "can you show me"
: (wh (vb | pos) | md? prp? imperative? prp?) dt?;

object options {backtrack=true;}
: part1 | part2;

part1 options {backtrack=true;}
: subpart1 | subpart2;

subpart1 options {backtrack=true;}
: exist_filter
| attribute_filter;

subpart2 options {backtrack=true;}
: object_with_prefix
| further_operation;

part2 options {backtrack=true;}
: subpart3 | subpart4;

subpart3 options {backtrack=true;}
: metric_filter
| graph_type;

subpart4 options {backtrack=true;}
: metadata
| connection;

object_with_prefix
: prps? (ascend | descend) cd? percent? attribute? in? dt? ((exist_filter)=>exist_filter | (attribute in? element)=>inside_attribute_filter | (element)=>attribute_filter)? in? metric // "top 3 call centers in sales"
{
	if ($cmd::mflag && $cmd::mClearFlag) {
		$cmd::mpNLPParser->hClear();
		$cmd::mClearFlag = false;
	}
	if ($attribute.text != "") {
		if ($prps.text != "") {
			string filter = $prps.text + $attribute.text;
			if ($cmd::mpMDSearch->CheckDiscontinuousFilter($cmd::mpNLPParser, $cmd::mvTerms, filter, $cmd::mObjectInfo)) {
				BSTR filterName = $cmd::mObjectInfo.name;
				wstring tmpName(filterName, SysStringLen(filterName));
				*($cmd::mpOutput) << "add Existing Filter(" << $cmd::converter.to_bytes(tmpName) << ")" << endl;
				if ($cmd::mflag) {
					GUID filterID = $cmd::mObjectInfo.id;
					$cmd::mpNLPParser->hCopyFilter($cmd::mpiNode, filterID, DssFunctionAnd, &$cmd::mpoNode);
					$cmd::mpiNode = $cmd::mpoNode;	
				}
			} else {
				unsigned int mi = atoi(string($attribute.text).substr(0, 2).c_str());
				unsigned int oi = $cmd::mvTermMDInds[mi];
				BSTR attributeName = $cmd::mvObjectInfos[oi].name;
				wstring tmpName(attributeName, SysStringLen(attributeName));
				*($cmd::mpOutput) << "add Attribute(" << $cmd::converter.to_bytes(tmpName) << ")"  << endl;
				if ($cmd::mflag) {
					mi = atoi(string($attribute.text).substr(0, 2).c_str());
					oi = $cmd::mvTermMDInds[mi];
					GUID attributeID = $cmd::mvObjectInfos[oi].id;
					$cmd::mpNLPParser->hPopulateReportTemplate(attributeID, DssTypeAttribute);
				}
			}
		} else {
			unsigned int mi = atoi(string($attribute.text).substr(0, 2).c_str());
			unsigned int oi = $cmd::mvTermMDInds[mi];
			BSTR attributeName = $cmd::mvObjectInfos[oi].name;
			wstring tmpName(attributeName, SysStringLen(attributeName));
			*($cmd::mpOutput) << "add Attribute(" << $cmd::converter.to_bytes(tmpName) << ")"  << endl;
			if ($cmd::mflag) {
				mi = atoi(string($attribute.text).substr(0, 2).c_str());
				oi = $cmd::mvTermMDInds[mi];
				GUID attributeID = $cmd::mvObjectInfos[oi].id;
				$cmd::mpNLPParser->hPopulateReportTemplate(attributeID, DssTypeAttribute);
			}
		}
	}
	
	unsigned int mi = atoi(string($metric.text).substr(0, 2).c_str());
	unsigned int oi = $cmd::mvTermMDInds[mi];
	BSTR metricName = $cmd::mvObjectInfos[oi].name;
	wstring tmpName(metricName, SysStringLen(metricName));
	*($cmd::mpOutput) << "add Metric(" << $cmd::converter.to_bytes(tmpName) << ")"  << endl;
	string rank;
	string percent;
	if ($cd.text != "")
		rank = ($cmd::mvTerms[atoi($cd.text.c_str())]).mTermTxt;
	else
		rank = "1";
	if ($percent.text != "")
		percent = "\%";
	else
		percent = "";
	if ($ascend.text != "") 
		*($cmd::mpOutput) << "add Filter(Rank " << rank << percent << ", Ascend) on Metric(" << $cmd::converter.to_bytes(tmpName) << ")"  << endl;
	else 
		*($cmd::mpOutput) << "add Filter(Rank " << rank << percent << ", Descend) on Metric(" << $cmd::converter.to_bytes(tmpName) << ")"  << endl;
	if ($cmd::mflag) {
		mi = atoi(string($metric.text).substr(0, 2).c_str());
		oi = $cmd::mvTermMDInds[mi];
		GUID metricID = $cmd::mvObjectInfos[oi].id;
		bool ibAscending;
		if ($ascend.text != "")
			ibAscending = true;
		else
			ibAscending = false;
		VARIANT iRank;
		if (percent != "") {
			iRank.vt = VT_R4;
			iRank.fltVal = atoi(rank.c_str()) / 100.00;
		} else {
			iRank.vt = VT_I4;
			iRank.lVal = atoi(rank.c_str());
		}
		$cmd::mpNLPParser->hMakeDerivedMetricRankQualification($cmd::mpiNode, metricID, ibAscending, iRank);
	}
};

metadata
: (attribute | metric)
{
	if ($cmd::mflag && $cmd::mClearFlag) {
		$cmd::mpNLPParser->hClear();
		$cmd::mClearFlag = false;
	}
	if ($attribute.text != "") {
		unsigned int mi = atoi(string($attribute.text).substr(0, 2).c_str());
		unsigned int oi = $cmd::mvTermMDInds[mi];
		BSTR attributeName = $cmd::mvObjectInfos[oi].name;
		wstring tmpName(attributeName, SysStringLen(attributeName));
		*($cmd::mpOutput) << "add Attribute(" << $cmd::converter.to_bytes(tmpName) << ")"  << endl;
	} else if ($metric.text != "") {
		unsigned int mi = atoi(string($metric.text).substr(0, 2).c_str());
		unsigned int oi = $cmd::mvTermMDInds[mi];
		BSTR metricName = $cmd::mvObjectInfos[oi].name;
		wstring tmpName(metricName, SysStringLen(metricName));
		*($cmd::mpOutput) << "add Metric(" << $cmd::converter.to_bytes(tmpName) << ")"  << endl;
	}
	if ($cmd::mflag) {
		if ($attribute.text != "") {
			unsigned int mi = atoi(string($attribute.text).substr(0, 2).c_str());
			unsigned int oi = $cmd::mvTermMDInds[mi];
			GUID attributeID = $cmd::mvObjectInfos[oi].id;
			$cmd::mpNLPParser->hPopulateReportTemplate(attributeID, DssTypeAttribute);
		} else if ($metric.text != "") {
			unsigned int mi = atoi(string($metric.text).substr(0, 2).c_str());
			unsigned int oi = $cmd::mvTermMDInds[mi];
			GUID metricID = $cmd::mvObjectInfos[oi].id;
			$cmd::mpNLPParser->hPopulateReportTemplate(metricID, DssTypeMetric);
		}			
	}	
};

connection
: (in | vb | to) dt?;

further_operation: {$cmd::mpMDSearch->CheckOperation($cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 1)}? vb prp? rp?
{
	*($cmd::mpOutput) << "Set clear flag to false!" << endl;
	unsigned int mi = atoi(string($vb.text).c_str());
	*($cmd::mpOutput) << "Operation: " << $cmd::mvTerms[mi].mTermTxt << endl;
	if ($cmd::mflag) {
		$cmd::mClearFlag = false;
	}
};
	
inside_attribute_filter : attribute in? (cc? element)+ // "category of books and movies"
{
	if ($cmd::mflag && $cmd::mClearFlag) {
		$cmd::mpNLPParser->hClear();
		$cmd::mClearFlag = false;
	}
	unsigned int mi = atoi(string($attribute.text).substr(0, 2).c_str());
	unsigned int oi = $cmd::mvTermMDInds[mi];
	BSTR attributeName = $cmd::mvObjectInfos[oi].name;
	GUID attributeID = $cmd::mvObjectInfos[oi].id;
	
	vector<string> vElementNames;
	vector<wstring> vElementIDs;
	
	string filter = string($inside_attribute_filter.text);
	bool flag = false;
	for (int i = 0; i < filter.size() / 2; i++) {
		unsigned int mi = atoi(filter.substr(2*i, 2).c_str());
 		if (flag == true && !($cmd::mvTerms[mi].IsINTerm() || $cmd::mvTerms[mi].IsCCTerm())) {
 			string elementName = $cmd::mvTerms[mi].mTermTxt;
 			oi = $cmd::mvTermMDInds[mi];
 			for (int j = 0; j < $cmd::mvvElementInfos[oi].size(); j++) {
 				string relatedAttributeName = $cmd::mvvElementInfos[oi][j].relatedName;
 				wstring wstr = MBase::UTF8ToWideChar(relatedAttributeName.c_str()).c_str();
 				wstring name(attributeName, SysStringLen(attributeName));
 				if (wstr == name) {
 					string elementID = $cmd::mvvElementInfos[oi][j].id;
 					vElementNames.push_back(elementName);
 					vElementIDs.push_back($cmd::converter.from_bytes(elementID));
 				}
 			}
 	 	}
 	 	if (filter.substr(2*i, 2) == string($attribute.text).substr(0, 2)) 
			flag = true;
 	}
	 
	if ($cmd::mflag) {
		if (vElementNames.size() > 0) {
	 		$cmd::mpNLPParser->hMakeInExpr($cmd::mpiNode, attributeID, DssFunctionIn, vElementIDs, DssFunctionAnd, &$cmd::mpoNode);
	 		$cmd::mpiNode = $cmd::mpoNode;
	 	} else {
	 		cout << "Warning! The attribute name in the filter and the element related attribute name are different!" << endl;
	 	}
	}
	
	wstring tmpName(attributeName, SysStringLen(attributeName));
	*($cmd::mpOutput) << "add Attribute(" << $cmd::converter.to_bytes(tmpName) << ")"  << endl;
	*($cmd::mpOutput) << "add Filter(Value in [";
	for (int i = 0; i < vElementNames.size(); i++) {
 		if (i != 0)
 			*($cmd::mpOutput) << ", ";
 	 	*($cmd::mpOutput) << vElementNames[i];
 	}
	*($cmd::mpOutput) << "]) on Attribute(" << $cmd::converter.to_bytes(tmpName) << ")" <<endl;
};

attribute_filter
: element (cc? element)*  // "NY", "NY and DC"
{
	if ($cmd::mflag && $cmd::mClearFlag) {
		$cmd::mpNLPParser->hClear();
		$cmd::mClearFlag = false;
	}
	string filter = string($attribute_filter.text);
	unsigned int mi = atoi(filter.substr(0, 2).c_str());
	unsigned int oi = $cmd::mvTermMDInds[mi];
 	string attributeName = $cmd::mvvElementInfos[oi][0].relatedName;
 	string attributeID = $cmd::mvvElementInfos[oi][0].relatedID;
 	
 	vector<string> vElementNames;
	vector<wstring> vElementIDs;
	wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
	GUID orGUID;
 	
 	for (int i = 0; i < filter.size() / 2; i++) {
 		mi = atoi(filter.substr(2*i, 2).c_str());
 		if (!$cmd::mvTerms[mi].IsCCTerm()) {
 			string elementName = $cmd::mvTerms[mi].mTermTxt;
 			oi = $cmd::mvTermMDInds[mi];
 			string elementID = $cmd::mvvElementInfos[oi][0].id;
 			vElementNames.push_back(elementName);
 			vElementIDs.push_back(converter.from_bytes(elementID));
 	 	}
 	}
 	
 	if ($cmd::mflag) {
	 	if (string2Guid(orGUID, attributeID)) {
	 		$cmd::mpNLPParser->hMakeInExpr($cmd::mpiNode, orGUID, DssFunctionIn, vElementIDs, DssFunctionAnd, &$cmd::mpoNode);
	 		$cmd::mpiNode = $cmd::mpoNode;
	 	}
	}
 	
 	*($cmd::mpOutput) << "add Attribute(" << attributeName << ")"  << endl;
 	*($cmd::mpOutput) << "add Filter(Value in [";
 	for (int i = 0; i < vElementNames.size(); i++) {
 		if (i != 0)
 			*($cmd::mpOutput) << ", ";
 	 	*($cmd::mpOutput) << vElementNames[i];
 	}
 	*($cmd::mpOutput) << "]) on Attribute(" + attributeName + ")" << endl;
};

metric_filter : metric op in? cd np? // "revenue greater than 1000000 dollars"
{
	if ($cmd::mflag && $cmd::mClearFlag) {
		$cmd::mpNLPParser->hClear();
		$cmd::mClearFlag = false;
	}
	unsigned int mi = atoi(string($metric.text).substr(0, 2).c_str());
	unsigned int oi = $cmd::mvTermMDInds[mi];
	BSTR metricName = $cmd::mvObjectInfos[oi].name;
	wstring tmpName(metricName, SysStringLen(metricName));
	mi = atoi(string($op.text).substr(0, 2).c_str());
	string operatorStr = $cmd::mvTerms[mi].mTermTxt;
	*($cmd::mpOutput) << "add Metric(" << $cmd::converter.to_bytes(tmpName) << ")"  << endl;
	mi = atoi($cd.text.c_str());
	*($cmd::mpOutput) << "add Filter(Value " << operatorStr << " " << $cmd::mvTerms[mi].mTermTxt << ") on Metric(" << $cmd::converter.to_bytes(tmpName) << ")"  << endl;
	if ($cmd::mflag) {
		mi = atoi(string($metric.text).substr(0, 2).c_str());
		oi = $cmd::mvTermMDInds[mi];
		GUID metricID = $cmd::mvObjectInfos[oi].id;
		mi = atoi($cd.text.c_str());
		string num = $cmd::mvTerms[mi].mTermTxt;
		char* arr = new char[num.size() + 1];
		unsigned i = 0;
		for (unsigned j = 0; j < num.size(); j++) {
			if (num[j] != ',')
				arr[i++] = num[j];
		}
		arr[i] = '\0';
		MBase::RefContainer<CComVariant> vConstants;
		vConstants.push_back(CComVariant(atoi(arr)));
		delete []arr;
		EnumDSSFunction functionType;
		if (operatorStr == "greater")
			functionType = DssFunctionGreater;
		$cmd::mpNLPParser->hMakeSingleMetricQualificationExpr($cmd::mpiNode, metricID, functionType, DssNodeConstant, vConstants, DssFunctionAnd, &$cmd::mpoNode);
		$cmd::mpiNode = $cmd::mpoNode;
	}
};

metric options {backtrack=true;}
: metric_part_2 | metric_part_1;

metric_part_1
: metric2 | metric1;

metric_part_2
: metric4 | metric3;

metric1: {$cmd::mpMDSearch->CheckMetric($cmd::mpNLPParser, $cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 1, $cmd::mObjectInfo)}? np
{
	string tmp = string($metric1.text);
	unsigned int si = atoi(tmp.substr(0, 2).c_str());
	unsigned int ei = atoi(tmp.substr(tmp.length()-2, 2).c_str()) + 1;
	$cmd::mpMDSearch->SetMDInfo($cmd::mObjectInfo, $cmd::mvTermMDInds, $cmd::mvObjectInfos, si, ei);
};

metric2: {$cmd::mpMDSearch->CheckMetric($cmd::mpNLPParser, $cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 2, $cmd::mObjectInfo)}? np (vb | np)
{
	string tmp = string($metric2.text);
	unsigned int si = atoi(tmp.substr(0, 2).c_str());
	unsigned int ei = atoi(tmp.substr(tmp.length()-2, 2).c_str()) + 1;
	$cmd::mpMDSearch->SetMDInfo($cmd::mObjectInfo, $cmd::mvTermMDInds, $cmd::mvObjectInfos, si, ei);
};

metric3: {$cmd::mpMDSearch->CheckMetric($cmd::mpNLPParser, $cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 3, $cmd::mObjectInfo)}? np in np
{
	string tmp = string($metric3.text);
	unsigned int si = atoi(tmp.substr(0, 2).c_str());
	unsigned int ei = atoi(tmp.substr(tmp.length()-2, 2).c_str()) + 1;
	$cmd::mpMDSearch->SetMDInfo($cmd::mObjectInfo, $cmd::mvTermMDInds, $cmd::mvObjectInfos, si, ei);
};

metric4: {$cmd::mpMDSearch->CheckMetric($cmd::mpNLPParser, $cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 4, $cmd::mObjectInfo)}? (np | jj) np (np | in) np
{
	string tmp = string($metric4.text);
	unsigned int si = atoi(tmp.substr(0, 2).c_str());
	unsigned int ei = atoi(tmp.substr(tmp.length()-2, 2).c_str()) + 1;
	$cmd::mpMDSearch->SetMDInfo($cmd::mObjectInfo, $cmd::mvTermMDInds, $cmd::mvObjectInfos, si, ei);
};

attribute options {backtrack=true;}
: attribute_part_2 | attribute_part_1;

attribute_part_1
: attribute2 | attribute1;

attribute_part_2
: attribute4 | attribute3;

attribute1
: {$cmd::mpMDSearch->CheckAttribute($cmd::mpNLPParser, $cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 1, $cmd::mObjectInfo)}? (np | jj | vb)
{
	string tmp = string($attribute1.text);
	unsigned int si = atoi(tmp.substr(0, 2).c_str());
	unsigned int ei = atoi(tmp.substr(tmp.length()-2, 2).c_str()) + 1;
	$cmd::mpMDSearch->SetMDInfo($cmd::mObjectInfo, $cmd::mvTermMDInds, $cmd::mvObjectInfos, si, ei);
};

attribute2
: {$cmd::mpMDSearch->CheckAttribute($cmd::mpNLPParser, $cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 2, $cmd::mObjectInfo)}? np (vb | np)
{
	string tmp = string($attribute2.text);
	unsigned int si = atoi(tmp.substr(0, 2).c_str());
	unsigned int ei = atoi(tmp.substr(tmp.length()-2, 2).c_str()) + 1;
	$cmd::mpMDSearch->SetMDInfo($cmd::mObjectInfo, $cmd::mvTermMDInds, $cmd::mvObjectInfos, si, ei);
};

attribute3
: {$cmd::mpMDSearch->CheckAttribute($cmd::mpNLPParser, $cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 3, $cmd::mObjectInfo)}? np np np
{
	string tmp = string($attribute3.text);
	unsigned int si = atoi(tmp.substr(0, 2).c_str());
	unsigned int ei = atoi(tmp.substr(tmp.length()-2, 2).c_str()) + 1;
	$cmd::mpMDSearch->SetMDInfo($cmd::mObjectInfo, $cmd::mvTermMDInds, $cmd::mvObjectInfos, si, ei);
};

attribute4
: {$cmd::mpMDSearch->CheckAttribute($cmd::mpNLPParser, $cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 4, $cmd::mObjectInfo)}? np np np np
{
	string tmp = string($attribute4.text);
	unsigned int si = atoi(tmp.substr(0, 2).c_str());
	unsigned int ei = atoi(tmp.substr(tmp.length()-2, 2).c_str()) + 1;
	$cmd::mpMDSearch->SetMDInfo($cmd::mObjectInfo, $cmd::mvTermMDInds, $cmd::mvObjectInfos, si, ei);
};

element options {backtrack=true;}
: element_part_2 | element_part_1;

element_part_1
: element2 | element1;

element_part_2
: element4 | element3;
	
element1: {$cmd::mpMDSearch->CheckElement($cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 1, $cmd::mvElementInfos)}? (cd | nnp | np)
{
	string tmp = string($element1.text);
	unsigned int si = atoi(tmp.substr(0, 2).c_str());
	unsigned int ei = atoi(tmp.substr(tmp.length()-2, 2).c_str()) + 1;
	$cmd::mpMDSearch->SetMDInfo($cmd::mvElementInfos, $cmd::mvTermMDInds, $cmd::mvvElementInfos, si, ei);
};

element2: {$cmd::mpMDSearch->CheckElement($cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 2, $cmd::mvElementInfos)}? nnp nnp
{
	string tmp = string($element2.text);
	unsigned int si = atoi(tmp.substr(0, 2).c_str());
	unsigned int ei = atoi(tmp.substr(tmp.length()-2, 2).c_str()) + 1;
	$cmd::mpMDSearch->SetMDInfo($cmd::mvElementInfos, $cmd::mvTermMDInds, $cmd::mvvElementInfos, si, ei);
};

element3: {$cmd::mpMDSearch->CheckElement($cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 3, $cmd::mvElementInfos)}? nnp nnp nnp
{
	string tmp = string($element3.text);
	unsigned int si = atoi(tmp.substr(0, 2).c_str());
	unsigned int ei = atoi(tmp.substr(tmp.length()-2, 2).c_str()) + 1;
	$cmd::mpMDSearch->SetMDInfo($cmd::mvElementInfos, $cmd::mvTermMDInds, $cmd::mvvElementInfos, si, ei);
};

element4: {$cmd::mpMDSearch->CheckElement($cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 4, $cmd::mvElementInfos)}? nnp nnp nnp nnp
{
	string tmp = string($element4.text);
	unsigned int si = atoi(tmp.substr(0, 2).c_str());
	unsigned int ei = atoi(tmp.substr(tmp.length()-2, 2).c_str()) + 1;
	$cmd::mpMDSearch->SetMDInfo($cmd::mvElementInfos, $cmd::mvTermMDInds, $cmd::mvvElementInfos, si, ei);
};

graph_type options {backtrack=true;}
: (graph_type2 | graph_type1)
{
	if ($cmd::mflag && $cmd::mClearFlag) {
		$cmd::mpNLPParser->hClear();
		$cmd::mClearFlag = false;
	}
	string tmp = string($graph_type.text);
	unsigned int si = atoi(tmp.substr(0, 2).c_str());
	unsigned int ei = atoi(tmp.substr(tmp.length()-2, 2).c_str()) + 1;
	string graphName = "";
	while (si < ei) {
		graphName += $cmd::mvTerms[si++].mTermTxt + " ";
	}
	graphName = graphName.substr(0, graphName.size() - 1);
	*($cmd::mpOutput) << "This report is a " << graphName << "!" << endl;
	if ($cmd::mflag) {
		graphName = tolower(graphName);
		if ($cmd::mpMDSearch->mmGraphConf.find(graphName) != $cmd::mpMDSearch->mmGraphConf.end())
			$cmd::mpNLPParser->hSetGraphType($cmd::mpMDSearch->mmGraphConf[graphName]);
	}
};

graph_type1
: {$cmd::mpMDSearch->CheckGraph($cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 1)}? (np | vb);

graph_type2
: {$cmd::mpMDSearch->CheckGraph($cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 2)}? np np;

exist_report
: {$cmd::mpMDSearch->CheckReport($cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), $cmd::mei, $cmd::mSearchResult)}? exist_report_name
{
	string reportName = $cmd::mSearchResult.name;
 	*($cmd::mpOutput) << "add Existing Report(" << reportName << ")" << endl;
};

exist_report_name: TERM+;

exist_filter
: {$cmd::mpMDSearch->CheckFilter($cmd::mpNLPParser, $cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), $cmd::mei, $cmd::mObjectInfo)}? exist_filter_name
{
	if ($cmd::mflag && $cmd::mClearFlag) {
		$cmd::mpNLPParser->hClear();
		$cmd::mClearFlag = false;
	}
	BSTR filterName = $cmd::mObjectInfo.name;
	wstring tmpName(filterName, SysStringLen(filterName));
	*($cmd::mpOutput) << "add Existing Filter(" << $cmd::converter.to_bytes(tmpName) << ")" << endl;
	if ($cmd::mflag) {
		GUID filterID = $cmd::mObjectInfo.id;
		$cmd::mpNLPParser->hCopyFilter($cmd::mpiNode, filterID, DssFunctionAnd, &$cmd::mpoNode);
		$cmd::mpiNode = $cmd::mpoNode;	
	}
};

exist_filter_name: prps np+ | jj np+ | dt np+; // "my region"

ascend
: {$cmd::mpMDSearch->CheckAscend($cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 1)}? (nn | jj | jjs);

descend
: {$cmd::mpMDSearch->CheckDescend($cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 1)}? (nn | jj | jjs);

percent
: {$cmd::mpMDSearch->CheckPercent($cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 1)}? nn;

imperative
: {$cmd::mpMDSearch->CheckImperative($cmd::mvTerms, atoi(this->LT(1)->getText().c_str()), 1)}? (vb | nn);

wh: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsWHTerm() }? TERM; // what, where
md: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsMDTerm() }? TERM; // can
vb: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsVBTerm() }? TERM; // verb
op: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsOPTerm() }? TERM; // <, >, =
cc: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsCCTerm() }? TERM; // and
np: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsNPTerm( ) }? TERM; // noun
jj: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsJJTerm() }? TERM; // adject
cd: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsCDTerm() }?  TERM; // number
in: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsINTerm() }? TERM; // in, by
dt: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsDTTerm() }? TERM; // the
prps: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsPRPSTerm() }? TERM; // my
prp: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsPRPTerm() }? TERM; // me, you
nn: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsNNTerm() }? TERM; // top, bottom
jjs: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsJJSTerm() }? TERM; // best, worst
nnp: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsNNPTerm() }? TERM; // "NY", "DC"
pos: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsPOSTerm() }? TERM; // 's
rp: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsRPTerm() }? TERM; // "down"
to: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsTOTerm() }? TERM; // "to"
punct: {$cmd::mvTerms[atoi(this->LT(1)->getText().c_str())].IsPUNCTTerm() }? TERM; // ".", "?"

TERM: '0'..'9'+;
WS: ' ' {skip();};