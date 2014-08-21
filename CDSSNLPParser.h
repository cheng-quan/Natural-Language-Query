#ifndef CDSSNLPParser_h
#define CDSSNLPParser_h 1

#include "resource.h"
#include "ATL/CComObjectRootExM.h"

#include "DSSNode/CDSSNode.h"
#include "Base/Defines/RefContainer.h"
#include "TextMining/TextAnalysis/TextAnalysis.h"

class CDSSNLPParser : public CComCoClass<CDSSNLPParser,&CLSID_DSSNLPParser>, 
#ifndef _USE_MSI_COM
	public ISupportErrorInfo,
#endif 
	public IDispatchImpl<IDSSNLPParser,&IID_IDSSNLPParser,&LIBID_DSSCOMMasterLib>,
	public ICDSSNLPParser,
	public CComObjectRootEx_M_NoLock
{
public:

	CDSSNLPParser();
	~CDSSNLPParser();
		
	STDMETHODIMP Init(IDSSReportInstance* ipReportInstance, IDSSUserRuntime* ipUserRuntime);
	STDMETHODIMP Init(ICDSSReportInstance* ipReportInstance, ICDSSUserRuntime* ipUserRuntime, ICDSSTemplate* ipDataTemplate, ICDSSFilter* ipDataFilter);
	STDMETHODIMP InterfaceSupportsErrorInfo(REFIID riid);
	// ytang, 2014-02-26
	STDMETHODIMP Parse(BSTR iText);
	HRESULT hMakeInExpr1(ICDSSNode * ipNode, DSS_ID& iAttributeID, EnumDSSFunction iFunctionType, vector<wstring> iElementIDs);
	HRESULT hMakeInExpr(ICDSSNode * ipNode, DSS_ID& iAttributeID, EnumDSSFunction iFunctionType, vector<wstring> iElementIDs, EnumDSSFunction iLogicOperator, ICDSSNode** oppNode);
	HRESULT hMakeAttributeQualificationExpr1(ICDSSNode * ipNode, DSS_ID& iAttributeID, DSS_ID& iForm, EnumDSSFunction iFunctionType,  MBase::RefContainer<CComVariant>& iConstants);
	HRESULT hMakeAttributeQualificationExpr(ICDSSNode * ipNode, DSS_ID& iAttributeID, DSS_ID& iForm, EnumDSSFunction iFunctionType,  MBase::RefContainer<CComVariant>& iConstants, EnumDSSFunction iLogicOperator, ICDSSNode** oppNode);
	HRESULT hMakeSingleMetricQualificationExpr1(ICDSSNode * ipNode, DSS_ID& iMetricID, EnumDSSFunction iFunctionType, EnumDSSNodeType iValueNodeType, MBase::RefContainer<CComVariant>& iConstants);
	HRESULT hMakeSingleMetricQualificationExpr(ICDSSNode * ipNode, DSS_ID& iMetricID, EnumDSSFunction iFunctionType, EnumDSSNodeType iValueNodeType, MBase::RefContainer<CComVariant>& iConstants, EnumDSSFunction iLogicOperator, ICDSSNode** oppNode);
	HRESULT hMakeDerivedMetricRankQualification1(ICDSSTemplate* iTemplate, ICDSSNode * ipNode, DSS_ID& iMetricID, bool ibAscending , VARIANT& iRank);	
	HRESULT hMakeDerivedMetricRankQualification(ICDSSNode * ipNode, DSS_ID& iMetricID, bool ibAscending , VARIANT& iRank);
	HRESULT hMakeDerivedMetricRankQualificationOnFilter(ICDSSNode * ipNode, DSS_ID& iMetricID, bool ibAscending , VARIANT& iRank, EnumDSSFunction iLogicOperator, ICDSSNode** oppNode);
	HRESULT hSetOperatorNodeProperty( IDSSOperator* ipOper, BSTR iPropName, VARIANT& iPropVal );
	HRESULT hSetOperatorNodeProperty( IDSSOperator* ipOper, Int32 iProp, VARIANT& iPropVal );
	HRESULT hPopulateReportTemplate(DSS_ID& iObjectID, EnumDSSObjectType iObjectType);
	HRESULT hCopyFilter(ICDSSNode* ipNode, DSS_ID& lpExistingFilter, EnumDSSFunction iLogicOperator, ICDSSNode** oppNode);
	HRESULT hGetContextFromRI(ICDSSReportInstance* ipInstance, ICDSSObjectContext** oppContext);
	HRESULT TraverseExpressionTree(ICDSSNode* ipNode);
	HRESULT hGetMDSearchServices(IDSSSearchServices** oppSearchServices);
	HRESULT UpdateExpressionTree(ICDSSNode * ipNode, EnumDSSFunction iLogicOperator, EnumDSSNodeType iNodeType, ICDSSNode** oppNode);
	HRESULT hCreateResultFolder(ICDSSFolder** oppFolder);
	HRESULT hMDSearch(BSTR iObjectName, vector<EnumDSSObjectType>iObjectTypes, ICDSSFolder** oppFolder);
	HRESULT hGetSearchExecuter(IDSSIndexSearchExecuter** oppIndexSearchExecuter);
	HRESULT hSetDisplayMode(EnumDSSDisplayMode iDisplayMode);
	HRESULT hSetGraphType(EnumDSSGraphType iGraphType);
	HRESULT hSetGraphType1(EnumDSSGraphType iGraphType, ICDSSTemplate* ipTemplate);
	HRESULT hGetGraphType(EnumDSSGraphType* opGraphType);
	HRESULT hCleanup();
	HRESULT hClear();

	DECLARE_REGISTRY_RESOURCEID(IDR_DSSNLPPARSER)
	BEGIN_COM_MAP(CDSSNLPParser)		
		COM_INTERFACE_ENTRY(ICDSSNLPParser)
		COM_INTERFACE_ENTRY(IDSSNLPParser)	
		COM_INTERFACE_ENTRY(IDispatch)
#ifndef _USE_MSI_COM
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
#endif  // _USE_MSI_COM
	END_COM_MAP()

	static TextAnalysis* TAInstance;
private:
	CComPtr<ICDSSReportInstance> mpReportInstance;
	CComPtr<ICDSSReportDefinition6> mpReportDefinition;
	CComPtr<ICDSSUserRuntime> mpUserRuntime;
	CComPtr<ICDSSTemplate> mpDataTemplate;
	CComPtr<ICDSSFilter> mpDataFilter;
};

#endif
