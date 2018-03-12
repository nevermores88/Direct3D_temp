#include "stdafx.h"
#include "Shader_3dapi_01_21.h"


CShader_3dapi_01_21::CShader_3dapi_01_21()
{
}


CShader_3dapi_01_21::~CShader_3dapi_01_21()
{
}

HRESULT CShader_3dapi_01_21::Create(LPDIRECT3DDEVICE9 pdev)
{
	CBaseClass::Create(pdev);

	HRESULT hr = 0;
	HWND hwnd;
	D3DDEVICE_CREATION_PARAMETERS ppm;

	m_pdev->GetCreationParameters(&ppm);
	hwnd = ppm.hFocusWindow;

	DWORD dwFlags = 0;

#if defined(_DEBUG) || defined(DEBUG)
	dwFlags |= D3DXSHADER_DEBUG;
#endif

	LPD3DXBUFFER pShader = NULL;
	LPD3DXBUFFER pError = NULL;

	hr = D3DXAssembleShaderFromFile("Ex01_21/Shader.vsh", NULL, NULL, dwFlags, &pShader, &pError);

	if (FAILED(hr))
	{
		int iSize = pError->GetBufferSize();
		void* ack = pError->GetBufferPointer();

		if (ack)
		{
			char* str = new char[iSize];
			sprintf(str, (const char*)ack, iSize);
			OutputDebugString(str);
			delete[] str;
		}
	}

	if (pShader)
	{
		hr = m_pdev->CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &m_pVertexShader);
		pShader->Release();
	}

	if (FAILED(hr))
		return E_FAIL;

	D3DVERTEXELEMENT9 vertex_decl[MAX_FVF_DECL_SIZE] = { 0 };
	D3DXDeclaratorFromFVF(Vertex::FVF, vertex_decl);
	if (FAILED(m_pdev->CreateVertexDeclaration(vertex_decl, &m_pFVF)))
		return E_FAIL;

	D3DXCreateTextureFromFile(m_pdev, "Ex01_21/earth.bmp", &m_pTex);

	INT	iSphereSegmentsNum = 128;
	m_iVertexNum = 2 * iSphereSegmentsNum*(iSphereSegmentsNum + 1);

	FLOAT fDeltaRingAngle = (D3DX_PI / iSphereSegmentsNum);
	FLOAT fDeltaSegAngle = (2.0f * D3DX_PI / iSphereSegmentsNum);

	m_pVertices = new Vertex[m_iVertexNum];
	Vertex* pVertices = m_pVertices;

	// Generate the group of rings for the sphere
	for (DWORD ring = 0; ring < iSphereSegmentsNum; ring++)
	{
		FLOAT r0 = 50 * sinf((ring + 0) * fDeltaRingAngle);
		FLOAT r1 = 50 * sinf((ring + 1) * fDeltaRingAngle);
		FLOAT y0 = 50 * cosf((ring + 0) * fDeltaRingAngle);
		FLOAT y1 = 50 * cosf((ring + 1) * fDeltaRingAngle);

		// Generate the group of segments for the current ring
		for (DWORD seg = 0; seg < (iSphereSegmentsNum + 1); seg++)
		{
			FLOAT x0 = r0 * sinf(seg * fDeltaSegAngle);
			FLOAT z0 = r0 * cosf(seg * fDeltaSegAngle);
			FLOAT x1 = r1 * sinf(seg * fDeltaSegAngle);
			FLOAT z1 = r1 * cosf(seg * fDeltaSegAngle);

			// Add two vertices to the strip which makes up the sphere
			// (using the transformed normal to generate texture coords)
			pVertices->p.x = x0;
			pVertices->p.y = y0;
			pVertices->p.z = z0;
			
			pVertices->u = -((FLOAT)seg) / iSphereSegmentsNum;
			pVertices->v = (ring + 0) / (FLOAT)iSphereSegmentsNum;

			pVertices->color = 0xFFFFFFFF;

			pVertices++;

			pVertices->p.x = x1;
			pVertices->p.y = y1;
			pVertices->p.z = z1;

			pVertices->u = -((FLOAT)seg) / iSphereSegmentsNum;
			pVertices->v = (ring + 1) / (FLOAT)iSphereSegmentsNum;

			pVertices->color = 0xFFFFFFFF;

			pVertices++;
		}
	}

	return S_OK;
}

void CShader_3dapi_01_21::Release()
{
	if (m_pVertexShader)
	{
		m_pVertexShader->Release();
		m_pVertexShader = NULL;
	}

	if (m_pFVF)
	{
		m_pFVF->Release();
		m_pFVF = NULL;
	}

	if (m_pTex)
	{
		m_pTex->Release();
		m_pTex = NULL;
	}

	if (m_pVertices)
	{
		delete[] m_pVertices;
		m_pVertices = NULL;
	}
}

void CShader_3dapi_01_21::Render()
{
	if (m_pdev)
	{
		D3DXMATRIX mtView;
		D3DXMATRIX mtProj;

		m_pdev->GetTransform(D3DTS_VIEW, &mtView);
		m_pdev->GetTransform(D3DTS_PROJECTION, &mtProj);

		m_pdev->SetRenderState(D3DRS_LIGHTING, FALSE);
		m_pdev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		m_pdev->SetRenderState(D3DRS_FOGENABLE, TRUE);
		m_pdev->SetRenderState(D3DRS_FOGCOLOR, D3DXCOLOR(1.0F, 1.0F, 0.2F, 1));

		m_pdev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_pdev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_pdev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		D3DXMATRIX	mtWorldView = m_mtWorld * mtView;
		D3DXMATRIX	mtWorldViewProj = m_mtWorld * mtView * mtProj;
		FLOAT	pFog[4];
		pFog[0] = 0.0f;						// begin
		pFog[1] = 1200.f;						// end
		pFog[2] = pFog[1] - pFog[0];	// delta = end - begin
		pFog[3] = 1.0f / pFog[2];			// delta reciprocal
		D3DXCOLOR	diff(1, 1, 1, 1);	// Diffuse color

		D3DXMatrixTranspose(&mtWorldView, &mtWorldView);
		D3DXMatrixTranspose(&mtWorldViewProj, &mtWorldViewProj);

		m_pdev->SetVertexShader(m_pVertexShader);
		m_pdev->SetVertexDeclaration(m_pFVF);

		m_pdev->SetVertexShaderConstantF(0, (FLOAT*)&mtWorldViewProj, 4);
		m_pdev->SetVertexShaderConstantF(4, (FLOAT*)&mtWorldView, 4);
		m_pdev->SetVertexShaderConstantF(10, (FLOAT*)&diff, 1);
		m_pdev->SetVertexShaderConstantF(12, (FLOAT*)pFog, 1);

		m_pdev->SetTexture(0, m_pTex);
		//m_pdev->SetFVF(Vertex::FVF);
		m_pdev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, m_iVertexNum - 2, m_pVertices, sizeof(Vertex));

		m_pdev->SetVertexShader(NULL);
		m_pdev->SetVertexDeclaration(NULL);
		m_pdev->SetTexture(0, NULL);

		m_pdev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

void CShader_3dapi_01_21::Update()
{
	if (m_pdev)
	{
		D3DXMATRIX mtY;
		D3DXMATRIX mtZ;
		
		static float c = 0.0f;
		c += 0.1f;

		D3DXMatrixIdentity(&m_mtWorld);
		D3DXMatrixRotationY(&mtY, D3DXToRadian(-c));
		D3DXMatrixRotationZ(&mtZ, D3DXToRadian(-23.5f));

		m_mtWorld = mtY * mtZ;
	}
}
