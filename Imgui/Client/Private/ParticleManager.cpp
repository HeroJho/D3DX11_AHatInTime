#include "stdafx.h"
#include "..\Public\ParticleManager.h"

#include "GameInstance.h"
#include "ToolManager.h"

#include "Particle.h"

IMPLEMENT_SINGLETON(CParticleManager)

CParticleManager::CParticleManager()
{
}

HRESULT CParticleManager::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);



	return S_OK;
}

void CParticleManager::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_M))
	{
		
		for (auto& DescTool : m_EffectInfos)
		{
			CParticleManager::Get_Instance()->Create_Effect(DescTool.Desc.cModelTag, DescTool.Desc.vParentPos,
				DescTool.Desc.vPos, DescTool.Desc.vDir, DescTool.Desc.vScale, DescTool.Desc.vScaleSpeed, DescTool.Desc.vRoation,
				DescTool.Desc.vDir, DescTool.Desc.fStopValue, DescTool.Desc.fSpeed,
				DescTool.Desc.bGravity, DescTool.Desc.fGravityValue, DescTool.Desc.fJumpPower, DescTool.Desc.fLifeTime

				, DescTool.eType, DescTool.iCount, DescTool.fRandPos, DescTool.fRandScale, DescTool.fRandScaleSpeed, 
				DescTool.vRandRotation, DescTool.fRandStopValue, DescTool.fRandSpeed, DescTool.fRandGravityValue, DescTool.fRandJumpPower, DescTool.fRandLifeTime);
		}


	}

	RELEASE_INSTANCE(CGameInstance);

}

void CParticleManager::Add_Particle()
{
	// 일단 기본 설정으로 추가한다 -> 수정은 이후에
	PARTICLETOOLDESC DescTool;
	DescTool.iID = m_iIDCount;



	lstrcpy(DescTool.Desc.cModelTag, TEXT("SmokeParticle"));
	DescTool.Desc.vParentPos = _float3(0.f, 0.f, 0.f);
	DescTool.Desc.vPos = _float3(0.f, 0.f, 0.f);
	DescTool.Desc.vLook = _float3(0.f, 1.f, 0.f);
	DescTool.Desc.vScale = _float3(1.f, 1.f, 1.f);
	DescTool.Desc.vScaleSpeed = _float3(1.f, 1.f, 1.f);
	DescTool.Desc.vRoation = _float3(0.f, 0.f, 0.f);
	DescTool.Desc.vDir = _float3(0.f, 1.f, 0.f);
	DescTool.Desc.fStopValue = 0.1f;
	DescTool.Desc.fSpeed = 3.f;
	DescTool.Desc.bGravity = true;
	DescTool.Desc.fGravityValue = 1.f;
	DescTool.Desc.fJumpPower = 3.f;
	DescTool.Desc.fLifeTime = 1.f;


	DescTool.eType = STATE_SIRCLE;
	DescTool.iCount = 1;
	DescTool.fRandPos = 0.f;
	DescTool.fRandScale = 0.f;
	DescTool.fRandScaleSpeed = 0.f;
	DescTool.vRandRotation = 0.f;
	DescTool.fRandStopValue = 0.f;
	DescTool.fRandSpeed = 0.f;
	DescTool.fRandGravityValue = 0.f;
	DescTool.fRandJumpPower = 0.f;
	DescTool.fRandLifeTime = 0.f;


	m_EffectInfos.push_back(DescTool);


	++m_iIDCount;
}

void CParticleManager::Remove_Particle()
{

}






void CParticleManager::Create_Particle(const TCHAR* cModelTag, _float3 vParentPos, _float3 vPos, _float3 vLook, _float3 vScale, _float3 vScaleSpeed, _float3 vRotaion, _float3 vDir, _float fStopValue, _float fSpeed, _bool bGravity, _float fGravityValue, _float fJumpPower, _float fLifeTime)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CParticle::PARTICLEDESC Desc;
	lstrcpy(Desc.cModelTag, cModelTag);
	Desc.vParentPos = vParentPos;
		
	Desc.vPos = vPos;
	XMStoreFloat3(&Desc.vLook, XMVector3Normalize(XMLoadFloat3(&vLook)));
	Desc.vScale = vScale;
	Desc.vRoation = vRotaion;


	XMStoreFloat3(&Desc.vDir, XMVector3Normalize(XMLoadFloat3(&vDir)));
	Desc.fSpeed = fSpeed;
	Desc.fStopValue = fStopValue;

	Desc.vScaleSpeed = vScaleSpeed;

	Desc.bGravity = bGravity;
	Desc.fGravityValue = fGravityValue;
	Desc.fJumpPower = fJumpPower;

	Desc.fLifeTime = fLifeTime;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Particle"), LEVEL_ANIMTOOL, TEXT("Layer_Particle"), &Desc)))
		return;

	RELEASE_INSTANCE(CGameInstance);
}

void CParticleManager::Create_Effect(const TCHAR * cModelTag, _float3 vParentPos, _float3 vPos, _float3 vLook, _float3 vScale, _float3 vScaleSpeed, _float3 vRotaion, _float3 vDir, _float fStopValue, _float fSpeed, _bool bGravity, _float fGravityValue, _float fJumpPower, _float fLifeTime, 
	TYPE eType, _uint iCount, _float fRandPos, _float fRandScale, _float fRandScaleSpeed, _float vRandRotation, _float fRandStopValue, _float fRandSpeed, _float fRandGravityValue, _float fRandJumpPower, _float fRandLifeTime)
{

	_float3		vRandomLook;

	for (_uint i = 0; i < iCount; ++i)
	{
		
		switch (eType)
		{
		case STATE_SIRCLE:
			Create_Effect_Sircle(&vRandomLook, vDir);
			break;
		case STATE_HARFSIRCLE:
			Create_Effect_HarfSircle(&vRandomLook, vDir);
			break;
		case STATE_DISK:
			Create_Effect_Disk(&vRandomLook);
			break;
		case STATE_PERP:
			Create_Effect_Perp(&vRandomLook);
			break;
		default:
			break;
		}



		// Dir, Pos
		XMStoreFloat3(&vRandomLook, XMVector3Normalize(XMLoadFloat3(&vRandomLook)));
		_float3		vRandomPos; XMStoreFloat3(&vRandomPos, XMLoadFloat3(&vPos) + XMLoadFloat3(&vRandomLook) * CToolManager::Get_Instance()->Get_RendomNum(0.f, fRandPos));


		// Scale Speed
		_float		fRandomScaleSpeedValueMin = vScaleSpeed.x - fRandScaleSpeed;
		if (0.f >= fRandomScaleSpeedValueMin)
			fRandomScaleSpeedValueMin = 0.f;
		_float		fRandomScaleSpeedValue = CToolManager::Get_Instance()->Get_RendomNum(fRandomScaleSpeedValueMin, vScaleSpeed.x + fRandScaleSpeed);
		_float3		vRandomScaleSpeed = _float3(fRandomScaleSpeedValue, fRandomScaleSpeedValue, fRandomScaleSpeedValue);

		// Scale
		_float		fRandomScaleValueMin = vScale.x - fRandScale;
		if (0.f >= fRandomScaleValueMin)
			fRandomScaleValueMin = 0.1f;
		_float		fRandomScaleValue = CToolManager::Get_Instance()->Get_RendomNum(fRandomScaleValueMin, vScale.x + fRandScale);
		_float3		vRandomScale = _float3(fRandomScaleValue, fRandomScaleValue, fRandomScaleValue);


		_float3		vRandRotation(0.f, 0.f, 0.f);

		
		// Speed Speed
		_float		fRandomStopValueMin = fStopValue - fRandStopValue;
		if (0.f >= fRandomStopValueMin)
			fRandomStopValueMin = 0.f;
		_float		fRandomStopValue = CToolManager::Get_Instance()->Get_RendomNum(fRandomStopValueMin, fStopValue + fRandStopValue);

		// Speed
		_float fRandomSpeedMin = fSpeed - fRandSpeed;
		if (0.f >= fRandomSpeedMin)
			fRandomSpeedMin = 0.f;
		_float		fRandomSpeed = CToolManager::Get_Instance()->Get_RendomNum(fRandomSpeedMin, fSpeed + fRandSpeed);


		// Gravity
		_float fGravityValueMin = fGravityValue - fRandGravityValue;
		if (0.f >= fGravityValueMin)
			fGravityValueMin = 0.f;
		_float		fRandomGravityValue = CToolManager::Get_Instance()->Get_RendomNum(fGravityValueMin, fGravityValue + fRandGravityValue);
		
		// Jump
		_float fRandomJumpPowerMin = fJumpPower - fRandJumpPower;
		if (0.f >= fRandomJumpPowerMin)
			fRandomJumpPowerMin = 0.f;
		_float		fRandomJumpPower = CToolManager::Get_Instance()->Get_RendomNum(fRandomJumpPowerMin, fJumpPower + fRandJumpPower);

		// Life Time
		_float fRandomLifeTimeMin = fLifeTime - fRandLifeTime;
		if (0.f >= fRandomLifeTimeMin)
			fRandomLifeTimeMin = 0.1f;
		_float		fRandomLifeTime = CToolManager::Get_Instance()->Get_RendomNum(fRandomLifeTimeMin, fLifeTime + fRandLifeTime);


		CParticleManager::Get_Instance()->Create_Particle(cModelTag, vParentPos,
			vRandomPos, vLook, vRandomScale, vRandomScaleSpeed, vRandRotation,
			vRandomLook, fRandomStopValue, fRandomSpeed,
			bGravity, fRandomGravityValue, fRandomJumpPower, fRandomLifeTime);
	}



}

void CParticleManager::Create_Effect_Sircle(_float3* Out_vDir, _float3 vDir)
{
	_vector vBaseDir = XMLoadFloat3(&vDir);

	_float fRandAnlge_X = CToolManager::Get_Instance()->Get_RendomNum(0.f, 360.f);
	_float fRandAnlge_Y = CToolManager::Get_Instance()->Get_RendomNum(0.f, 360.f);
	_float fRandAnlge_Z = CToolManager::Get_Instance()->Get_RendomNum(0.f, 360.f);

	_matrix mRX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), fRandAnlge_X);
	_matrix mRY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRandAnlge_Y);
	_matrix mRZ = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), fRandAnlge_Z);

	vBaseDir = XMVector3TransformNormal(vBaseDir, mRX);
	vBaseDir = XMVector3TransformNormal(vBaseDir, mRY);
	vBaseDir = XMVector3TransformNormal(vBaseDir, mRZ);

	XMStoreFloat3(Out_vDir, vBaseDir);
}

void CParticleManager::Create_Effect_HarfSircle(_float3 * Out_vDir, _float3 vDir)
{
	_vector vBaseDir = XMLoadFloat3(&vDir);

	_float fRandAnlge_X = CToolManager::Get_Instance()->Get_RendomNum(-90.f, 90.f);
	_float fRandAnlge_Y = CToolManager::Get_Instance()->Get_RendomNum(-90.f, 90.f);
	// _float fRandAnlge_Z = CToolManager::Get_Instance()->Get_RendomNum(-90.f, 90.f);

	_matrix mRX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fRandAnlge_X));
	_matrix mRY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fRandAnlge_Y));
	// _matrix mRZ = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), fRandAnlge_Z);

	vBaseDir = XMVector3TransformNormal(vBaseDir, mRX);
	vBaseDir = XMVector3TransformNormal(vBaseDir, mRY);
	// vBaseDir = XMVector3TransformNormal(vBaseDir, mRZ);

	XMStoreFloat3(Out_vDir, vBaseDir);
}

void CParticleManager::Create_Effect_Disk(_float3 * Out_vDir)
{
}

void CParticleManager::Create_Effect_Perp(_float3 * Out_vDir)
{
}






void CParticleManager::Free()
{

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
