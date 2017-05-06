#include "stdafx.h"
#include "GameApp.h"

#include "Shader_3dapi_02_12.h"
#include "Shader_3dapi_02_18.h"
#include "Shader_3dapi_02_19.h"
#include "Shader_3dapi_02_20.h"
#include "Shader_3dapi_02_21.h"
#include "Shader_3dapi_02_22.h"
#include "Shader_3dapi_02_23.h"
#include "Shader_3dapi_02_24.h"
#include "Shader_3dapi_02_25.h"
#include "Shader_3dapi_02_26.h"
#include "Shader_3dapi_02_27.h"

#include "Shader_3dapi_03_01.h"
#include "Shader_3dapi_03_02.h"
#include "Shader_3dapi_03_03.h"
#include "Shader_3dapi_03_04.h"
#include "Shader_3dapi_03_08.h"
#include "Shader_3dapi_03_09.h"
#include "Shader_3dapi_03_10.h"
#include "Shader_3dapi_03_11.h"
#include "TestShader.h"

CGameApp* CGameApp::m_pGameApp = NULL;

CGameApp::CGameApp()
{
}


CGameApp::~CGameApp()
{
}

HRESULT CGameApp::Create(HWND hWnd)
{
	HRESULT ret = CGameCore::Create(hWnd);
	SetCamera();

	m_pGameInstance = new CShader_3dapi_02_27();

	if (m_pGameInstance)
		m_pGameInstance->Create(m_pdev);

	m_pInputManager = g_pInput;
	m_pInputManager->Create(hWnd);

	m_pCameraManager = g_pCamera;
	m_pCameraManager->Create(m_pdev);

	m_pGrid = new CGrid();
	m_pGrid->Create(m_pdev);

	return ret;
}

void CGameApp::Release()
{
	CGameCore::Release();

	if (m_pGameInstance)
	{
		m_pGameInstance->Release();
		delete m_pGameInstance;
		m_pGameInstance = NULL;
	}

	if (!m_pGameApp)
	{
		delete m_pGameApp;
		m_pGameApp = NULL;
	}

	if (m_pInputManager)
	{
		m_pInputManager->Release();
		m_pInputManager = NULL;
	}

	if (m_pCameraManager)
	{
		m_pCameraManager->Release();
		m_pCameraManager = NULL;
	}

	if (m_pGrid)
	{
		delete m_pGrid;
		m_pGrid = NULL;
	}
}

void CGameApp::Render()
{
	if (NULL == m_pdev)
		return;

	m_pdev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(0, 128, 200), 1.0f, 0);

	if (SUCCEEDED(m_pdev->BeginScene()))
	{
		if (m_pCameraManager)
			m_pCameraManager->SetTransfrom();

		if (m_pGrid)
			m_pGrid->Render();

		if (m_pGameInstance)
			m_pGameInstance->Render();

		m_pdev->EndScene();
	}

	m_pdev->Present(NULL, NULL, NULL, NULL);
}

void CGameApp::Update()
{
	if (m_pInputManager)
	{
		m_pInputManager->Update();

		D3DXVECTOR3 vEps = m_pInputManager->GetMouseEps();

		if (m_pCameraManager)
		{
			if (vEps.z != 0.0f)
				m_pCameraManager->MoveForward(vEps.z*1.0f, 1.0f);

			if (m_pInputManager->KeyState('W'))
				m_pCameraManager->MoveForward(4.0f, 1.0f);

			if (m_pInputManager->KeyState('S'))
				m_pCameraManager->MoveForward(-4.0f, 1.0f);

			if (m_pInputManager->KeyState('A'))
				m_pCameraManager->MoveSide(-4.0f);

			if (m_pInputManager->KeyState('D'))
				m_pCameraManager->MoveSide(4.0f);

			if (m_pInputManager->BtnPress(1))
			{
				D3DXVECTOR3 vDelta = m_pInputManager->GetMouseEps();
				m_pCameraManager->Rotation(vDelta);
			}

			m_pCameraManager->Update();
		}
	}

	if (m_pGrid)
		m_pGrid->Update();

	if (m_pGameInstance)
		m_pGameInstance->Update();
}

void CGameApp::SetCamera()
{
	if (m_pdev)
	{
		RECT rect;

		GetClientRect(m_hWnd, &rect);

		m_VP.X = 0;
		m_VP.Y = 0;
		m_VP.Width = rect.right - rect.left;
		m_VP.Height = rect.bottom - rect.top;
		m_VP.MinZ = 0.0f;
		m_VP.MaxZ = 1.0f;

		m_Eye.x = 0.0f;
		m_Eye.y = 30.0f;
		m_Eye.z = -200.0f;

		m_At.x = 0.0f;
		m_At.y = 0.0f;
		m_At.z = 0.0f;

		m_Up.x = 0.0f;
		m_Up.y = 1.0f;
		m_Up.z = 0.0f;

		D3DXMatrixLookAtLH(&m_matView, &m_Eye, &m_At, &m_Up);
		m_pdev->SetTransform(D3DTS_VIEW, &m_matView);

		D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI / 4.0f, (float)m_VP.Width / (float)m_VP.Height, 1.0f, 2000.0f);
		m_pdev->SetTransform(D3DTS_PROJECTION, &m_matProj);

		m_pdev->SetViewport(&m_VP);
	}
}

CGameApp* CGameApp::GetInstance()
{
	if (!m_pGameApp)
	{
		m_pGameApp = new CGameApp();
	}

	return m_pGameApp;
}

HWND CGameApp::GetHWND()
{
	return m_hWnd;
}

const D3DVIEWPORT9* const CGameApp::GetViewPort()
{
	return &m_VP;
}
