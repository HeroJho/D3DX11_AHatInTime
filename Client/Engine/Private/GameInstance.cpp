#include "..\Public\GameInstance.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphic_Device(CGraphic_Device::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
	, m_pLevel_Manager(CLevel_Manager::Get_Instance())
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pComponent_Manager(CComponent_Manager::Get_Instance())
	, m_pTimer_Manager(CTimer_Manager::Get_Instance())	
	, m_pPicking(CPicking::Get_Instance())
	, m_pPipeLine(CPipeLine::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
	, m_pColliderManager(CColliderManager::Get_Instance())
	, m_pFont_Manager(CFont_Manager::Get_Instance())
	, m_pFrustum(CFrustum::Get_Instance())
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
{	

	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pColliderManager);
	Safe_AddRef(m_pPicking);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pTimer_Manager);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pGraphic_Device);

}


HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, HINSTANCE hInst, const GRAPHICDESC& GraphicDesc, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	/* ������ �����ϱ����� �ʱ�ȭ�۾��� ��ġ��. */
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	/* �׷��ȵ���̽� �ʱ�ȭ. */
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, ppDevice, ppContext)))
		return E_FAIL;

	/* �Է� �ʱ�ȭ. */
	if (FAILED(m_pInput_Device->Initialize(hInst, GraphicDesc.hWnd)))
		return E_FAIL;

	/* ���� �ʱ�ȭ. */

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

	if (FAILED(m_pPicking->Initialize(GraphicDesc.hWnd, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY, *ppDevice, *ppContext)))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pComponent_Manager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager || 
		nullptr == m_pObject_Manager || 
		nullptr == m_pInput_Device)
		return;

	m_pInput_Device->Update();

	m_pObject_Manager->Tick(fTimeDelta);

	m_pPipeLine->Update();

	m_pFrustum->Tick();

	m_pPicking->Tick();

	m_pObject_Manager->LateTick(fTimeDelta);

	m_pLevel_Manager->Tick(fTimeDelta);
}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager)
		return;

	m_pObject_Manager->Clear(iLevelIndex);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);	
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	return m_pGraphic_Device->Present();
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::Render_Level()
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Render();	
}

CGameObject * CGameInstance::Get_GameObjectPtr(_uint iLevelIndex, const _tchar * pLayerTag, _uint iLayerIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_GameObjectPtr(iLevelIndex, pLayerTag, iLayerIndex);
}

CComponent * CGameInstance::Get_ComponentPtr(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iLayerIndex)
{//												�� ���� �ε���        ã������ ���� ���̾� �ױ�,     ã������ ���� ���� �ױ�,     ã������ ���� ���̾ �ִ� ����Ʈ�� �� ��°���� (�÷��̾��� ��� 0)    
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_ComponentPtr(iLevelIndex, pLayerTag, pComponentTag, iLayerIndex);	
}

CGameObject * CGameInstance::Clone_GameObject(const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_GameObject(pPrototypeTag, pArg);	
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_GameObjectToLayer(const _tchar * pPrototypeTag, _uint iLevelIndex, const _tchar * pLayerTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObjectToLayer(pPrototypeTag, iLevelIndex, pLayerTag, pArg);
}

HRESULT CGameInstance::Add_GameObjectToLayer(const _tchar * pPrototypeTag, _uint iLevelIndex, const _tchar * pLayerTag, CGameObject ** pObj, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObjectToLayer(pPrototypeTag, iLevelIndex, pLayerTag, pObj, pArg);
}

list<CGameObject*>* CGameInstance::Get_LayerObjs(_uint iLevelIndex, const _tchar * pLayerTag)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_LayerObjs(iLevelIndex, pLayerTag);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);	
}

HRESULT CGameInstance::Check_Prototype(_int iLevelIndex, const _tchar * pPrototypeTag)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Check_Prototype(iLevelIndex, pPrototypeTag);
}


_float CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0f;
	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Add_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;
	return m_pTimer_Manager->Add_Timer(pTimerTag);
}

HRESULT CGameInstance::Update_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Update_Timer(pTimerTag);
}

_char CGameInstance::Get_DIKState(_uchar eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKState(eKeyID);
}

_char CGameInstance::Get_DIMKeyState(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMKeyState(eMouseKeyID);
}

_long CGameInstance::Get_DIMMoveState(DIMM eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMMoveState(eMouseMoveID);
}

_bool CGameInstance::Key_Down(_uchar eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Key_Down(eKeyID);
}

_bool CGameInstance::Key_Up(_uchar eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Key_Up(eKeyID);
}

_bool CGameInstance::Key_Pressing(_uchar eKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Key_Pressing(eKeyID);
}

_bool CGameInstance::Mouse_Down(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Mouse_Down(eMouseKeyID);
}

_bool CGameInstance::Mouse_Up(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Mouse_Up(eMouseKeyID);
}

_bool CGameInstance::Mouse_Pressing(DIMK eMouseKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Mouse_Pressing(eMouseKeyID);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eTransformState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eTransformState, TransformMatrix);
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eTransformState);	
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eTransformState);
	
}

_float4x4 CGameInstance::Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4_TP(eTransformState);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition();	
}

_matrix CGameInstance::Get_TransformMatrixInverse(CPipeLine::TRANSFORMSTATE eTransformState) const
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrixInverse(eTransformState);
}

const LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);	
}

HRESULT CGameInstance::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(pDevice, pContext, LightDesc);	
}

HRESULT CGameInstance::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc, CLight ** Out_pLight)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(pDevice, pContext, LightDesc, Out_pLight);
}

HRESULT CGameInstance::Remove_Light(CLight * pLight)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Remove_Light(pLight);
}

void CGameInstance::Clear_Light()
{
	if (nullptr == m_pLight_Manager)
		return;

	m_pLight_Manager->Clear_Light();
}

_float4x4 CGameInstance::Get_ShadowLightViewMatrix()
{
	if (nullptr == m_pLight_Manager)
		return _float4x4();

	return m_pLight_Manager->Get_ShadowLightViewMatrix();
}



void CGameInstance::Add_ColGroup(CColliderManager::COLLIDERGROUP eGroup, CGameObject * pObj)
{
	if (nullptr == m_pColliderManager)
		return;

	m_pColliderManager->Add_ColGroup(eGroup, pObj);
}

void CGameInstance::Calcul_ColGroup(CColliderManager::COLLIDERGROUP eGroupL, CColliderManager::COLLIDERGROUP eGroupR)
{
	if (nullptr == m_pColliderManager)
		return;

	m_pColliderManager->Calcul_ColGroup(eGroupL, eGroupR);
}

void CGameInstance::Clear_ColGroup()
{
	if (nullptr == m_pColliderManager)
		return;

	m_pColliderManager->Clear_ColGroup();
}


HRESULT CGameInstance::Add_Fonts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Fonts(pDevice, pContext, pFontTag, pFontFilePath);
}

HRESULT CGameInstance::Render_Fonts(const _tchar * pFontTag, const _tchar * pTextm, _float2 vPosition, _fvector vColor, _float fAngle, _float2 vOrigin, _float2 vScale)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Fonts(pFontTag, pTextm, vPosition, vColor, fAngle, vOrigin, vScale);
}

_bool CGameInstance::isIn_Frustum_WorldSpace(_fvector vWorldPos, float fRadius)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isIn_WorldSpace(vWorldPos, fRadius);
}

void CGameInstance::Get_WinToWorldPos(_float fX, _float fY, _float3* Out_vPos, _float3* Out_vDir)
{
	if (nullptr == m_pPicking)
		return;

	m_pPicking->Get_WinToWorldPos(fX, fY, Out_vPos, Out_vDir);
}


HRESULT CGameInstance::Bind_SRV(const _tchar * pTargetTag, CShader * pShader, const char * pConstantName)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Bind_SRV(pTargetTag, pShader, pConstantName);
}

void CGameInstance::Set_WipsData(_float * pWispRatios, _float4 * pWispPoss, _int pWispNum)
{
	if (nullptr == m_pTarget_Manager)
		return;

	m_pTarget_Manager->Set_WipsData(pWispRatios, pWispPoss, pWispNum);
}

void CGameInstance::Set_Dark(_bool bDark)
{
	if (nullptr == m_pTarget_Manager)
		return;

	m_pTarget_Manager->Set_Dark(bDark);
}

_bool CGameInstance::Get_Dark()
{
	if (nullptr == m_pTarget_Manager)
		return false;

	return m_pTarget_Manager->Get_Dark();
}

void CGameInstance::Set_PlayerPos(_float3 vPos)
{
	if (nullptr == m_pTarget_Manager)
		return;

	m_pTarget_Manager->Set_PlayerPos(vPos);
}

_float3 CGameInstance::Get_PlayerPos()
{
	if (nullptr == m_pTarget_Manager)
		return _float3();

	return m_pTarget_Manager->Get_PlayerPos();
}



void CGameInstance::Release_Engine()
{
	CFrustum::Get_Instance()->Destroy_Instance();

	CTarget_Manager::Get_Instance()->Destroy_Instance();

	CFont_Manager::Get_Instance()->Destroy_Instance();

	CGameInstance::Get_Instance()->Destroy_Instance();

	CColliderManager::Get_Instance()->Destroy_Instance();

	CComponent_Manager::Get_Instance()->Destroy_Instance();

	CObject_Manager::Get_Instance()->Destroy_Instance();

	CLevel_Manager::Get_Instance()->Destroy_Instance();

	CTimer_Manager::Get_Instance()->Destroy_Instance();

	CPicking::Get_Instance()->Destroy_Instance();

	CPipeLine::Get_Instance()->Destroy_Instance();

	CLight_Manager::Get_Instance()->Destroy_Instance();

	CInput_Device::Get_Instance()->Destroy_Instance();	
	
	CGraphic_Device::Get_Instance()->Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pColliderManager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pPicking);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device); 
	
}
