//==============================================================================================
//	FILENAME	: CDSSNLPParser.cpp
//	PROJECT		: COMHelper
//	AUTHOR		: Yanjie Chen
//	CREATION	: 2014-02-13
//	Copyright (C) MicroStrategy, Inc. 2014
//==============================================================================================

#include "StdAfx.h"
#include <sstream>
#include "CDSSNLPParser.h"
#include "DSSNode/CDSSNode.h"
#include "MDSearch.h"
#include "CMDLexer.hpp"
#include "CMDParser.hpp"
//#include "Base/Defines/RefContainer.h"

using namespace User;

TextAnalysis* CDSSNLPParser::TAInstance;

CDSSNLPParser::CDSSNLPParser():mpReportInstance(NULL),mpReportDefinition(NULL),mpUserRuntime(NULL)
{
}


CDSSNLPParser::~CDSSNLPParser()
{
}

STDMETHODIMP CDSSNLPParser::Init(IDSSReportInstance* ipReportInstance, IDSSUserRuntime* ipUserRuntime)
{
	SQIPtr(ICDSSReportInstance) lReportI(ipReportInstance);
	if(!lReportI) return E_NOINTERFACE;

	SQIPtr(ICDSSUserRuntime) lRuntime(ipUserRuntime);
	if (!lRuntime) return E_NOINTERFACE;

	Init(lReportI,lRuntime,NULL,NULL);
	return S_OK;
}

STDMETHODIMP CDSSNLPParser::Init(ICDSSReportInstance* ipReportInstance, ICDSSUserRuntime* ipUserRuntime, ICDSSTemplate* ipDataTemplate, ICDSSFilter* ipDataFilter)
{
	mpReportInstance = ipReportInstance;
	CComPtr<ICDSSReportDefinition> lReportDefinition;
	HRESULT hr = ipReportInstance->get_Definition(&lReportDefinition);
	if (FAILED(hr)) return hr;
	hr = lReportDefinition->QueryInterface(IID_ICDSSReportDefinition6, (void**)&mpReportDefinition);
	if (FAILED(hr)) return hr;	
	mpUserRuntime = ipUserRuntime;

	mpDataTemplate = ipDataTemplate;
	mpDataFilter = ipDataFilter;

	return S_OK;
}

STDMETHODIMP CDSSNLPParser::InterfaceSupportsErrorInfo (REFIID riid)
{
	static const IID* arr[] =
	{
		&IID_IDSSNLPParser,
	};
	for (Int32 i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (ATL::InlineIsEqualGUID(*arr[i],riid))
		//if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}

	return S_FALSE;


}

HRESULT CDSSNLPParser::hClear()
{
	HRESULT hr = S_OK;

	if (mpDataTemplate)
	{
		CComPtr<ICDSSAxis> lpDataTemplateColumnsAxis;

		hr = mpDataTemplate->get_Columns(&lpDataTemplateColumnsAxis);
		if (FAILED(hr)) return hr;

		lpDataTemplateColumnsAxis->Clear();
		
		CComPtr<ICDSSAxis> lpDataTemplateRowsAxis;

		hr = mpDataTemplate->get_Rows(&lpDataTemplateRowsAxis);
		if (FAILED(hr)) return hr;

		lpDataTemplateRowsAxis->Clear();

	}	

	if (mpDataFilter)
	{// clear the exisitng filter in case it's not empty
		CComPtr<ICDSSExpression> lpExpression;
		hr = mpDataFilter->get_Expression(&lpExpression);
		if (FAILED(hr)) return hr;	

		lpExpression->ClearExpression();
	}
	return S_OK;

}

// entry point, report manipulation passes in user input
STDMETHODIMP CDSSNLPParser::Parse(BSTR iText)
{	
	/*
	// temporary solution before GUI fixes workflow and to create empty report. 
	SQIPtr(ICDSSObjectInfo) lpReportDefObjectInfo(mpReportDefinition);
	if(!lpReportDefObjectInfo) return E_NOINTERFACE;
	
	CComPtr<ICDSSEmbeddedFolder> lpEmbeddedFolder;
	HRESULT hr = lpReportDefObjectInfo->get_Embedded(&lpEmbeddedFolder);
	if (FAILED(hr)) return hr;	

	CComPtr<ICDSSTemplate> lpViewTemplate;
	hr = mpReportDefinition->get_ViewTemplate(&lpViewTemplate);//lpCViewTemplate could be 0
	if FAILED(hr) return hr;

	if (!lpViewTemplate)
	{// create a new viewtemplate
		CComPtr<ICDSSObjectInfo> lpNewTemplateObjectInfo;	
		CComBSTR lName = L"NLPTemplate";
		hr = lpEmbeddedFolder->Add(DssTypeTemplate, lName, &lpNewTemplateObjectInfo);
		if (FAILED(hr)) return hr;	

		hr = lpNewTemplateObjectInfo->QueryInterface(IID_ICDSSTemplate, (void **) &lpViewTemplate);
		if( FAILED(hr) ) return hr;
	}		

	// by now lpViewTemplate should not be null	
	CComPtr<ICDSSAxis> lpViewTemplateColumnsAxis;

	hr = lpViewTemplate->get_Columns(&lpViewTemplateColumnsAxis);
	if (FAILED(hr)) return hr;

	lpViewTemplateColumnsAxis->Clear();
		
	CComPtr<ICDSSAxis> lpViewTemplateRowsAxis;

	hr = lpViewTemplate->get_Rows(&lpViewTemplateRowsAxis);
	if (FAILED(hr)) return hr;

	lpViewTemplateRowsAxis->Clear();

	if (mpDataTemplate)
	{
		CComPtr<ICDSSAxis> lpDataTemplateColumnsAxis;

		hr = mpDataTemplate->get_Columns(&lpDataTemplateColumnsAxis);
		if (FAILED(hr)) return hr;

		lpDataTemplateColumnsAxis->Clear();
		
		CComPtr<ICDSSAxis> lpDataTemplateRowsAxis;

		hr = mpDataTemplate->get_Rows(&lpDataTemplateRowsAxis);
		if (FAILED(hr)) return hr;

		lpDataTemplateRowsAxis->Clear();

	}			

	if (!mpDataFilter)
	{// create a new empty filter
		CComPtr<ICDSSObjectInfo> lpNLPFilterObjectInfo;
		CComBSTR lFilterName = L"NLPFilter";
		hr = lpEmbeddedFolder->Add(DssTypeFilter, lFilterName, &lpNLPFilterObjectInfo);
		if (FAILED(hr)) return hr;	
		
		hr = lpNLPFilterObjectInfo->QueryInterface(IID_ICDSSFilter, (void **) &mpDataFilter);
		if( FAILED(hr) ) return hr;
		if(mpDataFilter == NULL) return E_NOINTERFACE;

		mpReportDefinition->put_Filter(mpDataFilter);
	}	
	else
	{// clear the exisitng filter in case it's not empty
		CComPtr<ICDSSExpression> lpExpression;
		hr = mpDataFilter->get_Expression(&lpExpression);
		if (FAILED(hr)) return hr;	

		lpExpression->ClearExpression();
	}
	// POS code	
	*/
	
	if (!mpDataTemplate || !mpDataFilter)
		return E_UNEXPECTED;

	//HRESULT hr = hClear();
	//if (FAILED(hr))
	//	return hr;

	wstring iWString(iText);
	string iString = MBase::WideCharToUTF8(iWString.c_str()).c_str();

	MDSearch searchInstance;
	searchInstance.Init("./TAConf/NLPHelper/");

	//TextAnalysis TAInstance;
	//TAInstance.Init("./TAConf/TextAnalysis/lexalytics.properties.default");
	//TAInstance.CreateSession();

	vector<TAChunkTerm> vChunkTerms;
	TAInstance->Process(iString, vChunkTerms);
	//TAInstance.CloseSession();
	ofstream output("nlp_output.txt", std::ofstream::binary);
		
	string inputString;
	stringstream isString;
	for (int j = 0; j < vChunkTerms.size()-1; j++)
		isString << setfill('0') << setw(2) << j << ' ';
	isString << setfill('0') << setw(2) << vChunkTerms.size()-1;
	inputString = isString.str();
	CMDLexer::InputStreamType input((ANTLR_UINT8 *) inputString.c_str(), ANTLR_ENC_8BIT, (ANTLR_UINT32) inputString.length(), (ANTLR_UINT8 *) "test");
	//TLexer::InputStreamType input(fName, ANTLR_ENC_8BIT);
	CMDLexer lxr(&input);	    // CLexerNew is generated by ANTLR
	CMDParser::TokenStreamType tstream(ANTLR_SIZE_HINT, lxr.get_tokSource() );
	CMDParser psr(&tstream);  // CParserNew is generated by ANTLR3
	try {
		psr.cmd(vChunkTerms, &searchInstance, true, true, &output, this);
	} catch (exception &e) {
		cout << e.what() << endl;
	}

	HRESULT hr = hCleanup();
	if FAILED(hr) return hr;

    return S_OK;
}

// call center in new york
HRESULT CDSSNLPParser::hMakeInExpr1(ICDSSNode * ipNode, DSS_ID& iAttributeID, EnumDSSFunction iFunctionType, vector<wstring> iElementIDs)
{
	SQIPtr(ICDSSOperator) lpInOp(ipNode);
	if (!lpInOp) 
		return E_NOINTERFACE;	
		
	// use DssFunctionIn if user doesn't specify, so it could handle input like "New York Boston"
	if (iFunctionType == DssFunctionReserved)
		iFunctionType = DssFunctionIn;

	HRESULT hr = lpInOp->put_FunctionType( iFunctionType );
	if (FAILED(hr)) return hr;
	
	// Set the expression type of the node to indicate an elements list
	hr = ipNode->put_ExpressionType( DSSFilterListQual );
	if (FAILED(hr)) return hr;

	CComPtr<ICDSSNode> lpChildNode1;
	hr = ipNode->Add( DssNodeShortcut, 0, NULL, &lpChildNode1 );
	if (FAILED(hr)) return hr;

	SQIPtr(ICDSSShortcut) lpShortcut (lpChildNode1);
	if (!lpShortcut) return E_NOINTERFACE;

	// Set the attribute as the target
	hr = lpShortcut->put_TargetID(&iAttributeID, DssTypeAttribute);
	if (FAILED(hr)) return hr;

	CComPtr<ICDSSNode> lpChildNode2;
	hr = ipNode->Add( DssNodeElementsObject, 0, NULL, &lpChildNode2 );
	if (FAILED(hr)) return hr;

	SQIPtr(ICDSSElementsObject) lpElementsObject (lpChildNode2);
	if (!lpElementsObject) return E_NOINTERFACE;

	CComPtr<ICDSSElements> lpElements;
	hr = lpElementsObject->get_Elements( &lpElements );
	if (FAILED(hr)) return hr;

	// We should have a valid elements collection by now
	_ASSERTE( lpElements.p );

	for (int i = 0; i < iElementIDs.size(); ++i)
	{	
		CComBSTR lElementID(iElementIDs[i].c_str()); 		

		hr = SQIPtr(ICDSSElements2)(lpElements)->AddElementID(lElementID);
		if (FAILED(hr)) return hr;		
	}
	return S_OK;
}

// call center in new york
// enum EnumDSSFunction iFunctionType: DssFunctionIn = 22,   DssFunctionNotIn = 57,
// enum EnumDSSFunction iLogicOperator: DssFunctionAnd = 19,    DssFunctionOr = 20,    DssFunctionNot = 21,
HRESULT CDSSNLPParser::hMakeInExpr(ICDSSNode * ipNode, DSS_ID& iAttributeID, EnumDSSFunction iFunctionType, vector<wstring> iElementIDs, EnumDSSFunction iLogicOperator, ICDSSNode** oppNode)
{
	if (!oppNode)
	{
		return E_POINTER;
	}
	*oppNode = NULL;

	HRESULT hr = hPopulateReportTemplate(iAttributeID, DssTypeAttribute);
	if (FAILED(hr)) return hr;	
	
	CComPtr<ICDSSNode> lpNewNode;
	hr = UpdateExpressionTree(ipNode, iLogicOperator, DssNodeOperator, &lpNewNode);
	if (FAILED(hr)) return hr;

	hr = hMakeInExpr1(lpNewNode, iAttributeID, iFunctionType, iElementIDs);
	if (FAILED(hr))
		return hr;	
		
	*oppNode = lpNewNode;
	(*oppNode)->AddRef();

	return S_OK;
}


// eg. Region@ID = 2
HRESULT CDSSNLPParser::hMakeAttributeQualificationExpr1(ICDSSNode * ipNode, DSS_ID& iAttributeID, DSS_ID& iForm, EnumDSSFunction iFunctionType,  MBase::RefContainer<CComVariant>& iConstants)
{
	HRESULT hr = ipNode->put_ExpressionType( DSSFilterSingleBaseFormQual);
	if (FAILED(hr)) return hr;

	SQIPtr(ICDSSOperator) lpInOp(ipNode);
	if (!lpInOp) 
		return E_NOINTERFACE;	

	hr = lpInOp->put_FunctionType( iFunctionType );
	if (FAILED(hr)) return hr;		
	
	CComPtr<ICDSSNode> lpNewNode;
	hr = ipNode->Add(DssNodeFormShortcut, 0, NULL, &lpNewNode);
	if (FAILED(hr)) return hr;

	SQIPtr(ICDSSFormShortcut) lpFormShortcutPtr(lpNewNode);
	if (lpFormShortcutPtr == NULL) return E_NOINTERFACE;

	hr = lpFormShortcutPtr->put_AttributeID(&iAttributeID);
	if (FAILED(hr)) return hr;

	hr = lpFormShortcutPtr->put_FormID(&iForm);
	if (FAILED(hr)) return hr;

	for (int i = 0; i < iConstants.size(); ++i)
	{
		CComPtr<ICDSSNode> lpValueNode;
		// assume for now NodeType is always DssNodeConstant
		EnumDSSNodeType iValueNodeType = DssNodeConstant;
		hr = ipNode->Add(iValueNodeType, 0, NULL, &lpValueNode);
		if FAILED(hr) return hr;

		switch (iValueNodeType){
			// we'll add support for bigdecimal etc later, for now assume its always DssNodeConstant
			case DssNodeConstant:
				{
					SQIPtr(IDSSConstant) lpConstant (lpValueNode);
					if (!lpConstant) return E_NOINTERFACE;

					hr = lpConstant->put_Value(&iConstants[i]);
					if FAILED(hr) return hr;

					break;
				}
			case DssNodeBigDecimal:
				{
					SQIPtr(ICDSSBigDecimalNode) lpBigDecimalNode(lpValueNode);
					if (!lpBigDecimalNode) return E_NOINTERFACE;

					SQIPtr(IDSSBigDecimal) lpBigDecimal(iConstants[i].pdispVal);
					if (!lpBigDecimal) return E_NOINTERFACE;

					hr = lpBigDecimalNode->SetFromBigDecimalNS(lpBigDecimal);
					if (FAILED(hr)) return hr;

					break;
				}
			case DssNodeTime:
				{
					SQIPtr(IDSSTimeNode) lpTimeNode(lpValueNode);
					if (!lpTimeNode) return E_NOINTERFACE;

					// Get IDSSTime from time node
					CComPtr<IDSSTime> lpTime;
					hr = lpTimeNode->get_Time(&lpTime);
					if (FAILED(hr)) return hr;

					SQIPtr(IDSSTime) lpTimeValue(iConstants[i].pdispVal);
					if (!lpTimeValue) return E_NOINTERFACE;

					hr = lpTime->PopulateDefn(lpTimeValue);
					if (FAILED(hr)) return hr;

					break;
				}
			default:
				{
					// Value node type can only be constant, big decimal, or date time
					_ASSERTE(false);
					break;
				}
		}		
	}
	
	return S_OK;
}


// eg. Region@ID = 2
HRESULT CDSSNLPParser::hMakeAttributeQualificationExpr(ICDSSNode * ipNode, DSS_ID& iAttributeID, DSS_ID& iForm, EnumDSSFunction iFunctionType,  MBase::RefContainer<CComVariant>& iConstants, EnumDSSFunction iLogicOperator, ICDSSNode** oppNode)
{
	if (!oppNode)
	{
		return E_POINTER;
	}
	*oppNode = NULL;
	
	HRESULT hr = hPopulateReportTemplate(iAttributeID, DssTypeAttribute);
	if (FAILED(hr)) return hr;	
	
	CComPtr<ICDSSNode> lpNewNode;
	hr = UpdateExpressionTree(ipNode, iLogicOperator, DssNodeOperator, &lpNewNode);
	if (FAILED(hr)) return hr;

	hr = hMakeAttributeQualificationExpr1(lpNewNode, iAttributeID, iForm, iFunctionType, iConstants);
	if (FAILED(hr))
		return hr;	
		
	*oppNode = lpNewNode;
	(*oppNode)->AddRef();

	return S_OK;

}

// Revenue > 10000
HRESULT CDSSNLPParser::hMakeSingleMetricQualificationExpr1(ICDSSNode * ipNode, DSS_ID& iMetricID, EnumDSSFunction iFunctionType, EnumDSSNodeType iValueNodeType, MBase::RefContainer<CComVariant>& iConstants)
{	
	SQIPtr(ICDSSOperator) lpInOp(ipNode);
	if (!lpInOp) 
		return E_NOINTERFACE;

	HRESULT hr = ipNode->put_ExpressionType(DSSFilterSingleMetricQual);
	if FAILED(hr) return hr;

	hr = lpInOp->put_FunctionType(iFunctionType);
	if (FAILED(hr)) return hr;
	
	hr = ipNode->put_DimensionalityType(DssNodeDimtyOutputLevel);
	if FAILED(hr) return hr;

	CComPtr<ICDSSNode> lpMetricNode;
	hr = ipNode->Add(DssNodeShortcut, 0, NULL, &lpMetricNode);
	if FAILED(hr) return hr;

	SQIPtr(ICDSSShortcut) lpMetricShortcut (lpMetricNode);
	if (!lpMetricShortcut) return E_NOINTERFACE;

	/*if (ipMetricInfo)
	{
		hr = lpMetricShortcut->put_Target(ipMetricInfo);
		if (FAILED(hr)) return hr;*/

		/*CComPtr<ICDSSObjectInfo> lpObjInfo;//for testing
		hr = lpMetricShortcut->get_Target(&lpObjInfo);
		if (FAILED(hr)) return hr;*/
	/*}
	else
	{*/
		hr = lpMetricShortcut->put_TargetID(&iMetricID, DssTypeMetric);
		if (FAILED(hr)) return hr;
	/*}*/

	if (iValueNodeType != DssNodeReserved)
	{
		for (int i = 0; i < iConstants.size(); i++)
		{
			CComPtr<ICDSSNode> lpValueNode;
			hr = ipNode->Add(iValueNodeType, 0, NULL, &lpValueNode);
			if FAILED(hr) return hr;
			
			switch (iValueNodeType){
				case DssNodeConstant:
					{
						SQIPtr(IDSSConstant) lpConstant (lpValueNode);
						if (!lpConstant) return E_NOINTERFACE;

						hr = lpConstant->put_Value(&iConstants[i]);
						if FAILED(hr) return hr;

						break;
					}
				case DssNodeBigDecimal:
					{
						SQIPtr(ICDSSBigDecimalNode) lpBigDecimalNode(lpValueNode);
						if (!lpBigDecimalNode) return E_NOINTERFACE;

						SQIPtr(IDSSBigDecimal) lpBigDecimal(iConstants[i].pdispVal);
						if (!lpBigDecimal) return E_NOINTERFACE;

						hr = lpBigDecimalNode->SetFromBigDecimalNS(lpBigDecimal);
						if (FAILED(hr)) return hr;

						break;
					}
				case DssNodeTime:
					{
						SQIPtr(IDSSTimeNode) lpTimeNode(lpValueNode);
						if (!lpTimeNode) return E_NOINTERFACE;

						CComPtr<IDSSTime> lpTime;
						hr = lpTimeNode->get_Time(&lpTime);
						if (FAILED(hr)) return hr;

						SQIPtr(IDSSTime) lpTimeValue(iConstants[i].pdispVal);
						if (!lpTimeValue) return E_NOINTERFACE;
					
						hr = lpTime->PopulateDefn(lpTimeValue);
						if (FAILED(hr)) return hr;

						break;
					}
				default:
					{
						// Value node type can only be constant, big decimal, or date time
						_ASSERTE(false);
						break;
					}

		}
		
		}
	}

	return S_OK;
}

// Revenue > 10000
HRESULT CDSSNLPParser::hMakeSingleMetricQualificationExpr(ICDSSNode * ipNode, DSS_ID& iMetricID, EnumDSSFunction iFunctionType, EnumDSSNodeType iValueNodeType, MBase::RefContainer<CComVariant>& iConstants, EnumDSSFunction iLogicOperator, ICDSSNode** oppNode)
{
	if (!oppNode)
	{
		return E_POINTER;
	}
	*oppNode = NULL;
	
	HRESULT hr = hPopulateReportTemplate(iMetricID, DssTypeMetric);
	if (FAILED(hr)) return hr;	
	
	CComPtr<ICDSSNode> lpNewNode;
	hr = UpdateExpressionTree(ipNode, iLogicOperator, DssNodeOperator, &lpNewNode);
	if (FAILED(hr)) return hr;

	hr = hMakeSingleMetricQualificationExpr1(lpNewNode, iMetricID, iFunctionType, iValueNodeType, iConstants);
	if (FAILED(hr))
		return hr;
		
	*oppNode = lpNewNode;
	(*oppNode)->AddRef();

	return S_OK;
}

// new implmentation
// top 3 category by revenue
// ibAscending = false for "top 3", true for "bottom 3"
// 2014-06-16 add support for "top 25%"
HRESULT CDSSNLPParser::hMakeDerivedMetricRankQualification1(ICDSSTemplate* iTemplate, ICDSSNode * ipNode, DSS_ID& iMetricID, bool ibAscending , VARIANT& iRank)
{
	CComPtr<ICDSSNode> lpNode = ipNode;

	HRESULT hr = hPopulateReportTemplate(iMetricID, DssTypeMetric);
	if (FAILED(hr)) return hr;	

	SQIPtr(ICDSSObjectInfo6) licpTemplateInfo6(iTemplate);
	if (!licpTemplateInfo6) return E_NOINTERFACE;

	CComPtr<ICDSSObjectInfo> licpContainerInfo;
	hr = licpTemplateInfo6->get_Container(&licpContainerInfo); // the container should be report or document
	if (FAILED(hr))	return hr;
	if (!licpContainerInfo) return E_POINTER;

	
	CComPtr<ICDSSEmbeddedFolder> lpEmbeddedFolder;
	hr = licpContainerInfo->get_Embedded(&lpEmbeddedFolder);
	if (FAILED(hr)) return hr;	
		
	// create a derived metric ( Rank(revenue)(Descending) ) and then put in embedded folder	
	CComPtr<ICDSSObjectInfo> lpNLPDerivedMetricObjectInfo;
	BSTR lDerivedMetricName = L"NLPDerivedMetric";
	hr = lpEmbeddedFolder->Add(DssTypeMetric, lDerivedMetricName, &lpNLPDerivedMetricObjectInfo);
	if (FAILED(hr)) return hr;	
	
	SQIPtr(ICDSSMetric) lpNLPDerivedMetric(lpNLPDerivedMetricObjectInfo);
	if(!lpNLPDerivedMetric) return E_NOINTERFACE;

	CComPtr<ICDSSExpression> lpRankExpression;
	hr = lpNLPDerivedMetric->get_Expression(&lpRankExpression);
	if (FAILED(hr)) return hr;
	
	// put on metric limit for now, always put on the root node.
	CComPtr<ICDSSNode> lpNewRootNode;
	hr = lpRankExpression->AddRoot(DssNodeOperator, NULL, &lpNewRootNode);
	if (FAILED(hr)) return hr;	
	SQIPtr(ICDSSOperator) lpOperator( lpNewRootNode );
	if (!lpOperator) return hr;
	
	hr = lpOperator->put_FunctionType( DssFunctionRank );
	if (FAILED(hr)) return hr;
	
	// handle "top 25%" (iRank = 0.25)
	if ( (iRank.vt == VT_R4 && iRank.fltVal < 1.0) || (iRank.vt == VT_R8 && iRank.dblVal < 1.0) )
	{	
		CComPtr<ICDSSPropertySet> lpPropertySet;
		hr = lpOperator->get_Properties(&lpPropertySet);
		if FAILED(hr) return hr;      

		CComPtr<ICDSSProperty> lpProperty;
		hr = lpPropertySet->Item(RANK_FUNCTION_PROPERTY_INDEX_ByValue, &lpProperty);
		if FAILED(hr) return hr;                                                   
		
		CComVariant lValue;
		lValue.vt = VT_BOOL;
		lValue.boolVal = VARIANT_FALSE;

		hr = lpProperty->put_Value(&lValue);
		if FAILED(hr) return hr;  
	}

	hr = lpNewRootNode->put_ExpressionType(DSSFilterEmbedQual);
	if (FAILED(hr)) return hr;

	CComPtr<ICDSSNode> lpChildNode;
	hr = lpNewRootNode->Add(DssNodeShortcut, 0, NULL, &lpChildNode);
	if (FAILED(hr)) return hr;

	SQIPtr(ICDSSShortcut2) lpShortcutRoot(lpChildNode);
	if (!lpShortcutRoot)	return E_NOINTERFACE;

	hr = lpShortcutRoot->put_TargetID(&iMetricID, DssTypeMetric);
	if (FAILED(hr)) return hr;
	
	// Set properties on the Rank operator node.	
	CComVariant lcvPropVal (ibAscending);

	// Set IsDecomposable to true:
	hr = lpNLPDerivedMetric->put_IsDecomposableNS( DssDecomposableTrue );
	if( FAILED(hr) ) return hr;

	SQIPtr(IDSSOperator) lpOper(lpNewRootNode);
	if (!lpOper) return E_NOINTERFACE;	

	// Set the Ascending property:
	// need match property name string, may be slow 
	//hr = hSetOperatorNodeProperty( licpOper, L"ASC", lcvPropVal );
	// 4 is Ascending
	hr = hSetOperatorNodeProperty( lpOper, 4, lcvPropVal );
	if( FAILED(hr) ) return hr;	
	
	// template may or may not already have a metric	
	CComPtr<ICDSSTemplateMetrics> lpTemplateMetrics;
	hr = iTemplate->get_Metrics(&lpTemplateMetrics);
	if (FAILED(hr)) return hr;

	if(!lpTemplateMetrics)
	{// if template has no metrics, we need add a new one		
		CComPtr<ICDSSAxis> lpColumnsAxis;
		hr = iTemplate->get_Columns(&lpColumnsAxis);
		if (FAILED(hr)) return hr;

		CComPtr<ICDSSTemplateUnit> lpTemplateUnit;	

		hr = lpColumnsAxis->Add(DssTemplateMetrics, 0, &lpTemplateUnit);
		if (FAILED(hr)) return hr;

		hr = lpTemplateUnit->QueryInterface(IID_ICDSSTemplateMetrics, (void**) &lpTemplateMetrics);
		if (FAILED(hr)) return hr;	
				
		if (!lpTemplateMetrics)
			return E_NOINTERFACE;
	}
	
	// by now lpTemplateMetrics cannot be null
	CComPtr<ICDSSExpression> lpMetricLimitExpression;

	hr = lpTemplateMetrics->get_MetricLimit(&lpMetricLimitExpression);
	if (FAILED(hr)) return hr;

	// may change later to handle more complex expression?
	hr = lpMetricLimitExpression->ClearExpression();
	if (FAILED(hr)) return hr;

	// now populate the metric limit expression
	CComPtr<ICDSSNode> lRoot;
	DSS_ID lMetricID = GUID_NULL;
	EnumDSSFunction lFunctionType = DssFunctionReserved;
	
	hr = lpMetricLimitExpression->AddRoot(DssNodeOperator,NULL,&lRoot);
	if (FAILED(hr)) return hr;

	hr = lpNLPDerivedMetricObjectInfo->get_ID(&lMetricID);
	if (FAILED(hr)) return hr;

	lFunctionType = DssFunctionLessEqual;
	
	MBase::RefContainer<CComVariant> lConstants;
	lConstants.push_back(iRank);

	hr = hMakeSingleMetricQualificationExpr1(lRoot, lMetricID, lFunctionType, DssNodeConstant, lConstants);
	if (FAILED(hr)) return hr;	
	
	return S_OK;
}

// top 3 category by revenue
// ibAscending = false for "top 3", true for "bottom 3"
HRESULT CDSSNLPParser::hMakeDerivedMetricRankQualification(ICDSSNode * ipNode, DSS_ID& iMetricID, bool ibAscending , VARIANT& iRank)
{
	HRESULT hr = hMakeDerivedMetricRankQualification1(mpDataTemplate, ipNode, iMetricID, ibAscending, iRank);
	if (FAILED(hr)) return hr;	

	return S_OK;
}

// top 3 category by revenue
// new implementation, put it on filter expression instead of metric limit expression
// ibAscending = false for "top 3", true for "bottom 3"
HRESULT CDSSNLPParser::hMakeDerivedMetricRankQualificationOnFilter(ICDSSNode * ipNode, DSS_ID& iMetricID, bool ibAscending , VARIANT& iRank, EnumDSSFunction iLogicOperator, ICDSSNode** oppNode)
{
	if (!oppNode)
	{
		return E_POINTER;
	}
	*oppNode = NULL;
	
	HRESULT hr = hPopulateReportTemplate(iMetricID, DssTypeMetric);
	if (FAILED(hr)) return hr;	
			
	// create a derived metric ( Rank(revenue)(Descending) ) in embedded folder	
	SQIPtr(ICDSSObjectInfo)lpReportDefinitionObjectInfo(mpReportDefinition);
	if(!lpReportDefinitionObjectInfo) return E_NOINTERFACE;
	
	CComPtr<ICDSSEmbeddedFolder> lpEmbeddedFolder;
	hr = lpReportDefinitionObjectInfo->get_Embedded(&lpEmbeddedFolder);
	if (FAILED(hr)) return hr;			
	
	CComPtr<ICDSSObjectInfo> lpNLPDerivedMetricObjectInfo;
	BSTR lDerivedMetricName = L"NLPDerivedMetric";
	hr = lpEmbeddedFolder->Add(DssTypeMetric, lDerivedMetricName, &lpNLPDerivedMetricObjectInfo);
	if (FAILED(hr)) return hr;	
	
	SQIPtr(ICDSSMetric) lpNLPDerivedMetric(lpNLPDerivedMetricObjectInfo);
	if(!lpNLPDerivedMetric) return E_NOINTERFACE;

	CComPtr<ICDSSExpression> lpRankExpression;
	hr = lpNLPDerivedMetric->get_Expression(&lpRankExpression);
	if (FAILED(hr)) return hr;
	
	CComPtr<ICDSSNode> lpNewRootNode;
	hr = lpRankExpression->AddRoot(DssNodeOperator, NULL, &lpNewRootNode);
	if (FAILED(hr)) return hr;	
	SQIPtr(ICDSSOperator) lpOperator( lpNewRootNode );
	if (!lpOperator) return hr;

	hr = lpOperator->put_FunctionType( DssFunctionRank );
	if (FAILED(hr)) return hr;
		
	hr = lpNewRootNode->put_ExpressionType(DSSFilterEmbedQual);
	if (FAILED(hr)) return hr;

	CComPtr<ICDSSNode> lpChildNode;
	hr = lpNewRootNode->Add(DssNodeShortcut, 0, NULL, &lpChildNode);
	if (FAILED(hr)) return hr;

	SQIPtr(ICDSSShortcut2) lpShortcutRoot(lpChildNode);
	if (!lpShortcutRoot)	return E_NOINTERFACE;

	hr = lpShortcutRoot->put_TargetID(&iMetricID, DssTypeMetric);
	if (FAILED(hr)) return hr;

	hr = lpNLPDerivedMetric->put_IsDecomposableNS( DssDecomposableTrue );
	if( FAILED(hr) ) return hr;

	SQIPtr(IDSSOperator) lpOper(lpNewRootNode);
	if (!lpOper) return E_NOINTERFACE;	

	CComVariant lcvPropVal (ibAscending);

	// Set the Ascending property:
	// 4 is Ascending
	hr = hSetOperatorNodeProperty( lpOper, 4, lcvPropVal );
	if( FAILED(hr) ) return hr;	

	// now put on filter expression tree
	CComPtr<ICDSSNode> lpNewNode;
	hr = UpdateExpressionTree(ipNode, iLogicOperator, DssNodeOperator, &lpNewNode);
	if (FAILED(hr)) return hr;

	DSS_ID lDerivedMetricID = GUID_NULL;
	hr = lpNLPDerivedMetricObjectInfo->get_ID(&lDerivedMetricID);
	if (FAILED(hr)) return hr;
	
	EnumDSSFunction lFunctionType = DssFunctionLessEqual;

	MBase::RefContainer<CComVariant> lConstants;
	lConstants.push_back(iRank);
	
	hr = hMakeSingleMetricQualificationExpr1(lpNewNode, lDerivedMetricID,  lFunctionType, DssNodeConstant, lConstants);
	if (FAILED(hr)) return hr;		
			
	*oppNode = lpNewNode;
	(*oppNode)->AddRef();

	return S_OK;
}

// copied from HRESULT CDSSReportManipulation::hSetOperatorNodeProperty( ICDSSOperator* pOper, BSTR PropName, CComVariant PropVal )
// and HRESULT CDSSDocumentManipulator::hSetOperatorNodeProperty(ICDSSOperator* pOper, BSTR PropName, CComVariant PropVal)
HRESULT CDSSNLPParser::hSetOperatorNodeProperty( IDSSOperator* ipOper, BSTR iPropName, VARIANT& iPropVal )
{
	if( !ipOper ) return E_INVALIDARG;

	HRESULT hr = S_OK;
	
	CComPtr<IDSSPropertySet> lipPropSet;
	Int32 lnCount = 0,  n = 0 ;

	hr = ipOper->get_Properties( &lipPropSet );
	if( FAILED(hr) ) return hr;
	if( lipPropSet == NULL )
		return E_INVALIDARG; //...

	hr = lipPropSet->Count( &lnCount );
	if( FAILED(hr) ) return hr;

	// Look for a property with the given name:
	for ( n=1; n <= lnCount; n++ )
	{
		CComPtr<IDSSProperty> lipProperty;
		CComBSTR lcbsCrtName;

		{
			CComVariant lVar(n);
			hr = lipPropSet->Item( &lVar, (IDSSProperty**) &lipProperty );
			if( FAILED(hr) ) return hr;
		}

		hr = lipProperty->get_Name( &lcbsCrtName );
		if( FAILED(hr) ) return hr;

		// Compare given and current property name:

		if (::wcsicmp(iPropName, lcbsCrtName) == 0)
		{
			// Found, change value:
			hr = lipProperty->put_Value( &iPropVal );
			if( FAILED(hr) ) return hr;
			// Note:  the DSSProperty code checks for
			// type compatibility with the Default value.

			break;
		}
	}
	if( n > lnCount )
		return E_INVALIDARG; //...
	// property not found

	return S_OK;

} // hSetOperatorNodeProperty

HRESULT CDSSNLPParser::hSetOperatorNodeProperty( IDSSOperator* ipOper, Int32 iProp, VARIANT& iPropVal )
{
	CComPtr<IDSSPropertySet> lipPropSet;

	HRESULT hr = ipOper->get_Properties( &lipPropSet );
	if( FAILED(hr) ) return hr;
	if( lipPropSet == NULL )
		return E_INVALIDARG; 

	CComVariant lVar(iProp);
	CComPtr<IDSSProperty> lipProperty;
	hr = lipPropSet->Item( &lVar, (IDSSProperty**) &lipProperty );
	if( FAILED(hr) ) return hr;

	hr = lipProperty->put_Value( &iPropVal );
	if( FAILED(hr) ) return hr;

	return S_OK;
}

HRESULT CDSSNLPParser::hCopyFilter(ICDSSNode* ipNode, DSS_ID& iExistingFilterID, EnumDSSFunction iLogicOperator, ICDSSNode** oppNode)
{	
	if (!oppNode)
	{
		return E_POINTER;
	}
	*oppNode = NULL;

	CComPtr<ICDSSObjectContext> lpObjectContext;
	HRESULT hr = hGetContextFromRI(mpReportInstance, &lpObjectContext);
	if (FAILED(hr)) return hr;

	CComPtr<ICDSSObjectInfo3> lpFilterObject;
	hr = lpObjectContext->FindObjectByID(&iExistingFilterID, DssTypeFilter, NULL, 
		DssContextUnspecified, DssContextFlagDefault, &lpFilterObject);
	if (FAILED(hr))
		return hr;

	CComPtr<ICDSSFilter> lpExistingFilter;
	hr = lpFilterObject->QueryInterface(IID_ICDSSFilter, (void**) &lpExistingFilter);
	if (FAILED(hr)) return hr;
				
	// extract attribute/metric in the filter and put them on the template	
	CComPtr<ICDSSExpression> lpExpression;
	hr = lpExistingFilter->get_Expression(&lpExpression);
	if (FAILED(hr)) return hr;

	// traverse the tree to find all shortcut (attribute/metric), 
	// if the shortcut is another filter, continue the search in that tree 
	CComPtr<ICDSSNode> lpRootNode;
	hr = lpExpression->get_Root(&lpRootNode);
	if (FAILED(hr)) return hr;

	hr = TraverseExpressionTree(lpRootNode);
	if (FAILED(hr)) return hr;

	CComPtr<ICDSSNode> lpNewNode;
	hr = UpdateExpressionTree(ipNode, iLogicOperator, DssNodeShortcut, &lpNewNode);		
	if (FAILED(hr)) return hr;

	CComPtr<ICDSSShortcut> lpShortcut;
	hr = lpNewNode->QueryInterface(IID_ICDSSShortcut, (void**) &lpShortcut);
	if (FAILED(hr)) return hr;

	hr = lpShortcut->put_TargetID(&iExistingFilterID, DssTypeFilter);
	if (FAILED(hr))
		return hr;
		
	*oppNode = lpNewNode;
	(*oppNode)->AddRef();

	return S_OK;
}

// copied from CDSSDocumentInstance
HRESULT CDSSNLPParser::hGetContextFromRI(ICDSSReportInstance* ipInstance, ICDSSObjectContext** oppContext)
{
	if (!oppContext)
	{
		return E_POINTER;
	}
	*oppContext = NULL;

	// The binding table of the RI has method to access its object context
	CComPtr<ICDSSBindingTable> lBindingTablePtr;
	HRESULT hr = ipInstance->get_AppObjects(&lBindingTablePtr);
	if (FAILED(hr)) return hr;
	if (!lBindingTablePtr) return S_OK;

	SQIPtr(ICDSSBindingTable3) lBindingTable3Ptr(lBindingTablePtr);
	if (!lBindingTable3Ptr) return E_NOINTERFACE;

	// We now get the context from the RI
	return lBindingTable3Ptr->get_ObjectContext(oppContext);
}

HRESULT CDSSNLPParser::TraverseExpressionTree(ICDSSNode* ipNode)
{
	CComPtr<ICDSSNode> lpNode = ipNode;

	if (lpNode == NULL)
		return E_POINTER;

	Int32 lnChildren = 0;
	HRESULT hr = lpNode->Count(&lnChildren);
	if (FAILED(hr)) return hr;

	for (int i = 1; i <= lnChildren; ++i)
	{
		CComPtr<ICDSSNode> lpChildNode;
		hr = lpNode->Item(i, &lpChildNode);
		if (FAILED(hr)) return hr;

		EnumDSSNodeType lNodeType = DssNodeReserved;;
		hr = lpChildNode->get_Type(&lNodeType);
		if (FAILED(hr)) return hr;

		switch(lNodeType)
		{
		case DssNodeOperator:
			{
				hr = TraverseExpressionTree(lpChildNode);
				if (FAILED(hr)) return hr;
			}
			break;

		case DssNodeShortcut:
			{
				SQIPtr(ICDSSShortcut) licpShortcut(lpChildNode);
				if (!licpShortcut) return E_NOINTERFACE;	

				DSS_ID lObjectID = GUID_NULL;
				hr = licpShortcut->get_TargetID(&lObjectID);
				if (FAILED(hr)) return hr;

				EnumDSSObjectType lObjectType = DssTypeReserved;
				hr = licpShortcut->get_TargetType(&lObjectType);
				if (FAILED(hr)) return hr;
				
				if (lObjectType == DssTypeAttribute)
				{
					hr = hPopulateReportTemplate(lObjectID, DssTypeAttribute);
					if (FAILED(hr)) return hr;	
				}
				else if (lObjectType == DssTypeMetric)
				{
					hr = hPopulateReportTemplate(lObjectID, DssTypeMetric);
					if (FAILED(hr)) return hr;	
				}
				else if (lObjectType == DssTypeFilter)
				{
					/*
					SQIPtr(ICDSSNode) lpShortcutNode(lObjectInfo);
					if (!lpShortcutNode) 
						return E_NOINTERFACE;	
					
					hr = TraverseExpressionTree(lpShortcutNode);
					if (FAILED(hr)) return hr;
					*/
					SQIPtr(ICDSSFilter) lpFilter(lpChildNode);
					if (!lpFilter) return E_NOINTERFACE;

					CComPtr<ICDSSExpression> lpExpression;
					hr = lpFilter->get_Expression(&lpExpression);
					if (FAILED(hr)) return hr;

					CComPtr<ICDSSNode> lpRoot;
					hr = lpExpression->get_Root(&lpRoot);
					if (FAILED(hr))	return hr;			

					hr = TraverseExpressionTree(lpRoot);
					if (FAILED(hr)) return hr;

				}

			}
			break;

		case DssNodeConstant:
			{
				return S_OK;
			}
			break;

		case DssNodeElementsObject:
			{
				return S_OK;
			}
			break;		

		default:
			break;
		}


	}

	return hr;
}

HRESULT CDSSNLPParser::hPopulateReportTemplate(DSS_ID& iObjectID, EnumDSSObjectType iObjectType)
{	 
	HRESULT hr;	

	SQIPtr(ICDSSReportInstance8) licpReportInstance8(mpReportInstance);
	if (!licpReportInstance8) return E_NOINTERFACE;

	//CComPtr<ICDSSTemplate> lpTemplate;
	//// assume fow now its datatemplate that matters, need to change once GUI code is fixed
	//hr = licpReportInstance8->get_DataTemplate(&lpTemplate);
	//if (FAILED(hr))
	//	return hr;

	//CComPtr<ICDSSTemplate> lpViewTemplate;
	//// assume fow now its datatemplate that matters, need to change once GUI code is fixed
	//hr = licpReportInstance8->get_ViewTemplate(&lpViewTemplate);
	//if (FAILED(hr))
	//	return hr;
	
	EnumDSSGraphType lGraphType = DssGraphTypeReserved;
	hr = hGetGraphType(&lGraphType);
	if (FAILED(hr))		return hr;

	// check whether the same attribute/metric already exists on the template before put in
	switch (lGraphType) 
	{
		case DssGraphTypePies:
		{
			// for pie chart we put all attribute and metrics on column for now 	
			CComPtr<ICDSSAxis> lpColumnAxis;
			hr = mpDataTemplate->get_Columns(&lpColumnAxis);
			if (FAILED(hr)) return hr;

			if(iObjectType == DssTypeAttribute)
			{	
				int lAttributeCount = 0;
				hr = lpColumnAxis->Count(&lAttributeCount);
				if (FAILED(hr)) return hr;

				for (int i = 0; i < lAttributeCount; ++i)
				{
					CComPtr<ICDSSTemplateUnit> lTemplateUnit;
					// Item() takes 1-based index as it calls mMembers[iIndex-1]
					hr = lpColumnAxis->Item(i+1, &lTemplateUnit);
					if (FAILED(hr)) return hr;

					DSS_ID lTemplateAttributeID = GUID_NULL;
					hr = lTemplateUnit->get_ID(&lTemplateAttributeID);
					if (FAILED(hr)) return hr;

					if (lTemplateAttributeID == iObjectID)
					{// already exists on the template, return without doing anything
						return S_OK;
					}
				}
				// no match found, add attribute to template
				CComPtr<ICDSSTemplateUnit> lpTemplateUnit;
				hr = lpColumnAxis->Add(DssTemplateAttribute, 0, &lpTemplateUnit);
				if (FAILED(hr)) return hr;

				CComPtr<ICDSSTemplateAttribute> lpTemplateAttribute;
				hr = lpTemplateUnit->QueryInterface(IID_ICDSSTemplateAttribute, (void**) &lpTemplateAttribute);
				if (FAILED(hr)) return hr;

				hr = lpTemplateAttribute-> put_AttributeID(&iObjectID);
				if (FAILED(hr)) return hr;
			}// if(iObjectType == DssTypeAttribute)
			else if (iObjectType == DssTypeMetric)
			{
				CComPtr<ICDSSTemplateMetrics> lpTemplateMetrics;
				hr = mpDataTemplate->get_Metrics(&lpTemplateMetrics);		
				if (FAILED(hr)) return hr;

				if (!lpTemplateMetrics)
				{// template has no metrics	
					CComPtr<ICDSSTemplateUnit>  lpTemplateUnit;
					hr = lpColumnAxis->Add(DssTemplateMetrics, 0, &lpTemplateUnit);
					if (FAILED(hr)) return hr;

					hr = lpTemplateUnit->QueryInterface(IID_ICDSSTemplateMetrics, (void**)&lpTemplateMetrics);
					if (FAILED(hr)) return hr;
				}

				// by now lpTemplateMetrics should not be null
				int lMetricCount = 0;
				hr = lpTemplateMetrics->Count(&lMetricCount);
				if (FAILED(hr)) return hr;	

				for (int i = 0; i < lMetricCount; ++i)
				{
					CComPtr<ICDSSTemplateMetric> lpTemplateMetric;
					// Item() takes 1-based index as it calls mMetrics[Index-1]
					hr = lpTemplateMetrics->Item(i+1, &lpTemplateMetric);
					if (FAILED(hr)) return hr;

					DSS_ID lTemplateMetricID = GUID_NULL;
					hr = lpTemplateMetric->get_MetricID(&lTemplateMetricID);
					if (FAILED(hr)) return hr;

					if (ATL::InlineIsEqualGUID(lTemplateMetricID, iObjectID))
					{// already exists on the template, return without doing anything
						return S_OK;
					}
				}
		
				CComPtr<ICDSSTemplateMetric> lpDummyTemplateMetricCPtr;
				hr = lpTemplateMetrics->AddID(&iObjectID, &lpDummyTemplateMetricCPtr);
				if (FAILED(hr)) return hr;
			}

			break;
		}//case DssGraphTypePies:

		default:
		{
			if(iObjectType == DssTypeAttribute)
			{		
				CComPtr<ICDSSAxis> lRowsAxisCPtr;
				hr = mpDataTemplate->get_Rows(&lRowsAxisCPtr);
				if (FAILED(hr)) return hr;

				int lAttributeCount = 0;
				hr = lRowsAxisCPtr->Count(&lAttributeCount);
				if (FAILED(hr)) return hr;

				for (int i = 0; i < lAttributeCount; ++i)
				{
					CComPtr<ICDSSTemplateUnit> lTemplateUnit;
					// Item() takes 1-based index as it calls mMembers[iIndex-1]
					hr = lRowsAxisCPtr->Item(i+1, &lTemplateUnit);
					if (FAILED(hr)) return hr;

					DSS_ID lTemplateAttributeID = GUID_NULL;
					hr = lTemplateUnit->get_ID(&lTemplateAttributeID);
					if (FAILED(hr)) return hr;

					if (lTemplateAttributeID == iObjectID)
					{// already exists on the template, return without doing anything
						return S_OK;
					}
				}
				// no match found, add attribute to template
				CComPtr<ICDSSTemplateUnit> lTemplateUnitCPtr;
				hr = lRowsAxisCPtr->Add(DssTemplateAttribute, 0, &lTemplateUnitCPtr);
				if (FAILED(hr)) return hr;

				CComPtr<ICDSSTemplateAttribute> lTemplateAttributeCPtr;
				hr = lTemplateUnitCPtr->QueryInterface(IID_ICDSSTemplateAttribute, (void**) &lTemplateAttributeCPtr);
				if (FAILED(hr)) return hr;

				hr = lTemplateAttributeCPtr-> put_AttributeID(&iObjectID);
				if (FAILED(hr)) return hr;
			}
			else if (iObjectType == DssTypeMetric)
			{
				CComPtr<ICDSSTemplateMetrics> lpTemplateMetrics;
				hr = mpDataTemplate->get_Metrics(&lpTemplateMetrics);		
				if (FAILED(hr)) return hr;

				if (!lpTemplateMetrics)
				{// template has no metrics				
					CComPtr<ICDSSAxis> lpColumnsAxis;
					hr = mpDataTemplate->get_Columns(&lpColumnsAxis);
					if (FAILED(hr)) return hr;
	
					CComPtr<ICDSSTemplateUnit>  lpTemplateUnit;
					hr = lpColumnsAxis->Add(DssTemplateMetrics, 0, &lpTemplateUnit);
					if (FAILED(hr)) return hr;

					hr = lpTemplateUnit->QueryInterface(IID_ICDSSTemplateMetrics, (void**)&lpTemplateMetrics);
					if (FAILED(hr)) return hr;
				}

				// by now lpTemplateMetrics should not be null
				int lMetricCount = 0;
				hr = lpTemplateMetrics->Count(&lMetricCount);
				if (FAILED(hr)) return hr;	

				for (int i = 0; i < lMetricCount; ++i)
				{
					CComPtr<ICDSSTemplateMetric> lpTemplateMetric;
					// Item() takes 1-based index as it calls mMetrics[Index-1]
					hr = lpTemplateMetrics->Item(i+1, &lpTemplateMetric);
					if (FAILED(hr)) return hr;

					DSS_ID lTemplateMetricID = GUID_NULL;
					hr = lpTemplateMetric->get_MetricID(&lTemplateMetricID);
					if (FAILED(hr)) return hr;

					if (ATL::InlineIsEqualGUID(lTemplateMetricID, iObjectID))
					{// already exists on the template, return without doing anything
						return S_OK;
					}
				}
		
				CComPtr<ICDSSTemplateMetric> lpDummyTemplateMetricCPtr;
				hr = lpTemplateMetrics->AddID(&iObjectID, &lpDummyTemplateMetricCPtr);
				if (FAILED(hr)) return hr;
			}

			break;
		}//default:
	}

	/*CComPtr<ICDSSObjectInfo> licpVTInfo;
	hr = lpViewTemplate->get_Info(&licpVTInfo);
	if (FAILED(hr)) return hr;

	CComPtr<ICDSSObjectInfo> licpDTInfo;
	hr = lpTemplate->get_Info(&licpDTInfo);
	if (FAILED(hr)) return hr;

	hr = licpVTInfo->PopulateObject(licpDTInfo, DssSourceTotalObject|DssSourceCopyIdentical);
	if (FAILED(hr)) return hr;*/

	return S_OK;
}

HRESULT CDSSNLPParser::hGetMDSearchServices(IDSSSearchServices** oppSearchServices)
{	
	if (!oppSearchServices)
	{
		return E_POINTER;
	}
	*oppSearchServices = NULL;

	SQIPtr(ICDSSObjectInfo) lpReportDefObjectInfo(mpReportDefinition);
	if(!lpReportDefObjectInfo) return E_NOINTERFACE;

	CComPtr<ICDSSSource> lpObjectServer;
	HRESULT hr = lpReportDefObjectInfo->get_Source(&lpObjectServer);
	if (FAILED(hr)) return hr;

	CComPtr<ICDSSSession> lpSession;
	hr = lpObjectServer->get_Session(&lpSession);
	if (FAILED(hr)) return hr;

	CComPtr<ICDSSDataSource> lpDataSource;
	hr = lpSession->get_DataSource(&lpDataSource);
	if (FAILED(hr)) return hr;
		
	SQIPtr(IDSSDataSource6) lpDataSource6(lpDataSource);
	if (!lpDataSource6) return E_NOINTERFACE;
	
	return lpDataSource6->get_SearchServices(oppSearchServices);
	
}

// for exisiting filter iNodeType = DssNodeShortcut, everything else iNodeType = DssNodeOperator
HRESULT CDSSNLPParser::UpdateExpressionTree(ICDSSNode * ipNode, EnumDSSFunction iLogicOperator, EnumDSSNodeType iNodeType, ICDSSNode** oppNode)
{	
	if (!oppNode)
	{
		return E_POINTER;
	}
	*oppNode = NULL;

	CComPtr<ICDSSNode> lpNode = ipNode;

	if (!mpDataFilter)
		return E_UNEXPECTED;

	CComPtr<ICDSSExpression> lpExpression;
	HRESULT hr = mpDataFilter->get_Expression(&lpExpression);
	if (FAILED(hr)) return hr;	

	if (lpNode == NULL)
	{// default: root node 
		hr = lpExpression->get_Root(&lpNode);
		if (FAILED(hr))	return hr;

		if (lpNode == NULL)
		{// it's an empty expression
			hr = lpExpression->AddRoot(iNodeType, NULL, &lpNode);
			if (FAILED(hr))	return hr;
			
			*oppNode = lpNode;
			(*oppNode)->AddRef();

			return S_OK;
		}
	}
	
	// now lpNode cannot be null
	CComPtr<ICDSSNode> lpParentNode;
	hr = lpNode->get_Parent(&lpParentNode);
	if (FAILED(hr)) return hr;
		
	CComPtr<ICDSSNode> lpNewNode;

	if (lpParentNode == NULL) // lpNode is root
	{// create a new root node and put the old root i.e. lpNode under the new root
		hr = lpExpression->AddRoot(DssNodeOperator, NULL, &lpNewNode);
		if (FAILED(hr))	return hr;		
	}	
	else
	{
		// internal node
		// create a new branch (AND, OR) node and insert it under old parent node. 
		hr = lpExpression->AddRoot(DssNodeOperator, NULL, &lpNewNode);
		if (FAILED(hr)) return hr;
	}
	
	hr = lpNewNode->put_ExpressionType(DSSFilterBranchQual);
	if (FAILED(hr)) return hr;

	SQIPtr(ICDSSOperator) lpNewNodeOp(lpNewNode);
	if (!lpNewNodeOp) 
		return E_NOINTERFACE;	

	hr = lpNewNodeOp->put_FunctionType(iLogicOperator);
	if (FAILED(hr)) return hr;
	
	// move the old subtree to under the new branch node
	CComPtr<ICDSSNode> lpChildNode1;
	hr = lpNewNode->AddMove(lpNode, 0, &lpChildNode1);
	if (FAILED(hr)) return hr;

	// create a new operator (in/not in) node and attach it to the newly created AND/OR node 
	CComPtr<ICDSSNode> lpChildNode2;
	hr = lpNewNode->Add(iNodeType, 0, NULL, &lpChildNode2);
	if (FAILED(hr)) return hr;	
		
	*oppNode = lpChildNode2;
	(*oppNode)->AddRef();

	return S_OK;
}

HRESULT CDSSNLPParser::hCreateResultFolder(ICDSSFolder** oppFolder)
{		
	if (!oppFolder)
	{
		return E_POINTER;
	}
	*oppFolder = NULL;

	SQIPtr(ICDSSObjectInfo) lpReportDefObjectInfo(mpReportDefinition);
	if(!lpReportDefObjectInfo) return E_NOINTERFACE;

	CComPtr<ICDSSSource> lpObjectServer;
	HRESULT hr = lpReportDefObjectInfo->get_Source(&lpObjectServer);
	if (FAILED(hr)) return hr;

	// Create an an out-of-cache search folder object
	CComPtr<ICDSSObjectInfo> lpSearchFolder;
	hr = lpObjectServer->NewObject(DssTypeSearchFolder, DssSourceDoNotCache, NULL, &lpSearchFolder);
	if (FAILED(hr)) return hr;
	
	SQIPtr(ICDSSFolder) lpNewFolder(lpSearchFolder);
	if(!lpNewFolder) return E_NOINTERFACE;

	*oppFolder = lpNewFolder;
	(*oppFolder)->AddRef();

	return S_OK;
}

HRESULT CDSSNLPParser::hMDSearch(BSTR iObjectName, vector<EnumDSSObjectType>iObjectTypes, ICDSSFolder** oppFolder)
{
	if (!oppFolder)
	{
		return E_POINTER;
	}
	*oppFolder = NULL;

	CComPtr<ICDSSFolder> lpResultFolder;
	HRESULT hr = hCreateResultFolder(&lpResultFolder);
	if (FAILED(hr)) return hr;

	CComPtr<IDSSIndexSearchExecuter> lpSearchExecuter;
	hr = hGetSearchExecuter(&lpSearchExecuter);
	if (FAILED(hr)) return hr;

	SQIPtr(IDSSObjectInfo) lpReportDefObjectInfo(mpReportDefinition);
	if(!lpReportDefObjectInfo) return E_NOINTERFACE;

	CComPtr<IDSSSource> lpObjectSource;
	hr = lpReportDefObjectInfo->get_Source(&lpObjectSource);
	if (FAILED(hr)) return hr;

	SQIPtr(IDSSUserRuntime) lpUserRuntime(mpUserRuntime);
	if(!lpUserRuntime) return E_NOINTERFACE;

	CComPtr<IDSSObjectInfo> lpSearchObjectInfo;
	hr = lpObjectSource->NewObject(DssTypeSearch, DssSourceDoNotCache, lpUserRuntime, &lpSearchObjectInfo);
	if (FAILED(hr))	return hr;
	
	SQIPtr(IDSSSearch) lpSearch(lpSearchObjectInfo);
	if (!lpSearch) return E_NOINTERFACE;	

	lpSearch->put_NamePattern(iObjectName);
	CComPtr<IDSSTypes> lObjectTypes;

	lpSearch->get_Types(&lObjectTypes);
	if (FAILED(hr))	return hr;	
	for (int i = 0; i < iObjectTypes.size(); ++i)
	{
		hr = lObjectTypes->Add(iObjectTypes[i]);
		if (FAILED(hr))	return hr;
	}

	Int32 lStart = 1;
	Int32 lSearchType = 0;// >= 0 means new MD search in MSICommandUtilityXML::XMLSearch?
	CComBSTR lFields;

	SQIPtr(IDSSFolder) lpFolder(lpResultFolder);
	hr = lpSearchExecuter->Search(lpSearch,lStart,lSearchType, lFields, lpUserRuntime, lpFolder);
	if (FAILED(hr)) return hr;

	*oppFolder = lpResultFolder;
	(*oppFolder)->AddRef();

	return S_OK;
}

HRESULT CDSSNLPParser::hGetSearchExecuter(IDSSIndexSearchExecuter** oppIndexSearchExecuter)
{
	CComPtr<IDSSSearchServices> lpSearchServices;

	HRESULT hr = hGetMDSearchServices(&lpSearchServices);
	if (FAILED(hr)) return hr;
	
	return lpSearchServices->get_IndexSearchExecuter(oppIndexSearchExecuter);	
}

HRESULT CDSSNLPParser::hSetDisplayMode(EnumDSSDisplayMode iDisplayMode)
{
	SQIPtr(IDSSReportDefinition) lpReportDefinition(mpReportDefinition);
	if(!lpReportDefinition) return E_NOINTERFACE;

	HRESULT hr = lpReportDefinition->put_DisplayMode(iDisplayMode);
	if (FAILED(hr)) return hr;
	
	return S_OK;
}

HRESULT CDSSNLPParser::hSetGraphType(EnumDSSGraphType iGraphType)
{
	HRESULT hr = hSetGraphType1(iGraphType, mpDataTemplate);
	if (FAILED(hr)) return hr;

	return S_OK;
}


HRESULT CDSSNLPParser::hSetGraphType1(EnumDSSGraphType iGraphType, ICDSSTemplate* ipTemplate)
{
	SQIPtr(ICDSSTemplate6) lpViewTemplate(ipTemplate);
	if (!lpViewTemplate) return E_NOINTERFACE;

	CComPtr<ICDSSGraphStyle> lpGraphStyle;
	// assume default is always DssGraphTypeVertical_Bar_Side_by_Side
	HRESULT hr = lpViewTemplate->get_GraphStyleByTypeNS(DssGraphTypeVertical_Bar_Side_by_Side, &lpGraphStyle);
	if (FAILED(hr)) return hr;

	hr = lpGraphStyle->put_GraphTypeNS(iGraphType);
	if (FAILED(hr)) return hr;
	
	return S_OK;
}

HRESULT CDSSNLPParser::hGetGraphType(EnumDSSGraphType* opGraphType)
{
	if (!opGraphType)
		return E_POINTER;

	*opGraphType = DssGraphTypeReserved;

	SQIPtr(ICDSSTemplate6) lpTemplate6(mpDataTemplate);
	if (!lpTemplate6) return E_NOINTERFACE;

	CComPtr<ICDSSGraphStyle> lpGraphStyle;
	// assume default is always DssGraphTypeVertical_Bar_Side_by_Side
	HRESULT hr = lpTemplate6->get_GraphStyleByTypeNS(DssGraphTypeVertical_Bar_Side_by_Side, &lpGraphStyle);
	if (FAILED(hr)) return hr;

	hr = lpGraphStyle->get_GraphTypeNS(opGraphType);
	if (FAILED(hr)) return hr;

	return S_OK;
}

HRESULT CDSSNLPParser::hCleanup()
{// organize attributes and metrics according to graphtype
	EnumDSSGraphType lGraphType = DssGraphTypeReserved;
	HRESULT hr = hGetGraphType(&lGraphType);
	if (FAILED(hr))		return hr;

	MBase::RefContainer<DSS_ID> lvAttributeIDs;
	MBase::RefContainer<DSS_ID> lvMetricIDs;

	CComPtr<ICDSSAxis> lRowsAxisCPtr;
	hr = mpDataTemplate->get_Rows(&lRowsAxisCPtr);
	if (FAILED(hr)) return hr;

	int lRowObjectCount = 0;
	if (lRowsAxisCPtr)
	{		
		hr = lRowsAxisCPtr->Count(&lRowObjectCount);
		if (FAILED(hr)) return hr;
	}

	for (int i = 1; i <= lRowObjectCount; ++i)
	{		
		CComPtr<ICDSSTemplateUnit> lTemplateUnit;		
		hr = lRowsAxisCPtr->Item(i, &lTemplateUnit);
		if (FAILED(hr)) return hr;

		DSS_ID lObjectID = GUID_NULL;
		hr = lTemplateUnit->get_ID(&lObjectID);
		if (FAILED(hr)) return hr;		

		EnumDSSTemplateUnitType lUnitType = DssTemplateReserved;
		hr = lTemplateUnit->get_UnitType(&lUnitType);
		if (FAILED(hr)) return hr;

		if (lUnitType == DssTemplateAttribute)
		{
			lvAttributeIDs.push_back(lObjectID);
		}
		else if (lUnitType == DssTemplateMetrics)
		{
			lvMetricIDs.push_back(lObjectID);
		}		
	}
	hr = lRowsAxisCPtr->Clear();
	if (FAILED(hr)) return hr;


	CComPtr<ICDSSAxis> lColumnAxisCPtr;
	hr = mpDataTemplate->get_Columns(&lColumnAxisCPtr);
	if (FAILED(hr)) return hr;

	int lColumnObjectCount = 0;

	if (lColumnAxisCPtr)
	{		
		hr = lColumnAxisCPtr->Count(&lColumnObjectCount);
		if (FAILED(hr)) return hr;
	}

	for (int i = 1; i <= lColumnObjectCount; ++i)
	{		
		CComPtr<ICDSSTemplateUnit> lTemplateUnit;		
		hr = lColumnAxisCPtr->Item(i, &lTemplateUnit);
		if (FAILED(hr)) return hr;

		EnumDSSTemplateUnitType lUnitType = DssTemplateReserved;
		hr = lTemplateUnit->get_UnitType(&lUnitType);
		if (FAILED(hr)) return hr;

		if (lUnitType == DssTemplateAttribute)
		{
			DSS_ID lObjectID = GUID_NULL;
			hr = lTemplateUnit->get_ID(&lObjectID);
			if (FAILED(hr)) return hr;		

			lvAttributeIDs.push_back(lObjectID);
		}
		else if (lUnitType == DssTemplateMetrics)
		{
			SQIPtr(ICDSSTemplateMetrics) licpMetrics(lTemplateUnit);
			if (!licpMetrics)
				return E_NOINTERFACE;

			Int32 lnMetrics = 0;
			hr = licpMetrics->Count(&lnMetrics);
			if (FAILED(hr)) return hr;

			for (Int32 j = 1; j <= lnMetrics; j++)
			{
				CComPtr<ICDSSTemplateMetric> licpTemplateMetric;
				hr = licpMetrics->Item(j, &licpTemplateMetric);
				if (FAILED(hr)) return hr;

				DSS_ID lMetricID;
				hr = licpTemplateMetric->get_MetricID(&lMetricID);
				if (FAILED(hr)) return hr;

				lvMetricIDs.push_back(lMetricID);
			}
		}		
	}
	hr = lColumnAxisCPtr->Clear();
	if (FAILED(hr)) return hr;

	/*
	CComPtr<ICDSSTemplateMetrics> lpTemplateMetrics;
	hr = mpDataTemplate->get_Metrics(&lpTemplateMetrics);		
	if (FAILED(hr)) return hr;

	int lMetricCount = 0;
	if(lpTemplateMetrics)
	{
		hr = lpTemplateMetrics->Count(&lMetricCount);
		if (FAILED(hr)) return hr;	
	}
	for (int i = 1; i <= lMetricCount; ++i)
	{
		CComPtr<ICDSSTemplateMetric> lpTemplateMetric;
		hr = lpTemplateMetrics->Item(i, &lpTemplateMetric);
		if (FAILED(hr)) return hr;

		DSS_ID lTemplateMetricID = GUID_NULL;
		hr = lpTemplateMetric->get_MetricID(&lTemplateMetricID);
		if (FAILED(hr)) return hr;

		lvMetricIDs.push_back(lTemplateMetricID);

		lpTemplateMetrics->Remove(i);
	}
	*/
	for (int i = 0; i < lvAttributeIDs.size(); ++i)
	{
		hr = hPopulateReportTemplate(lvAttributeIDs[i], DssTypeAttribute);
		if (FAILED(hr)) return hr;
	}

	for (int i = 0; i < lvMetricIDs.size(); ++i)
	{
		hr = hPopulateReportTemplate(lvMetricIDs[i], DssTypeMetric);
		if (FAILED(hr)) return hr;
	}

	return S_OK;
}