#include "stdafx.h"
#include "Basic_3dapi_08_02.h"


CBasic_3dapi_08_02::CBasic_3dapi_08_02()
{
}


CBasic_3dapi_08_02::~CBasic_3dapi_08_02()
{
}

HRESULT CBasic_3dapi_08_02::Create(LPDIRECT3DDEVICE9 pdev)
{
	CBaseClass::Create(pdev);

	// Setup Tiger
	{
		m_pMeshS = NULL;
		if (FAILED(D3DXLoadMeshFromX("Ex08_01/Tiger.x", D3DXMESH_SYSTEMMEM, m_pdev, NULL, NULL, NULL, NULL, &m_pMeshS)))
			return E_FAIL;

		D3DXCreateTextureFromFile(m_pdev, "Ex08_01/tiger.bmp", &m_pTigerTex);


		LPD3DXMESH pMeshD = NULL;
		m_pMeshS->CloneMeshFVF(D3DXMESH_SYSTEMMEM, Vertex::FVF, m_pdev, &pMeshD);
		//pMeshS->Release();

		m_iNumVertex = pMeshD->GetNumVertices();
		m_iNumIndex = pMeshD->GetNumFaces();

		//	DWORD dFVF = pMdlD->GetFVF();

		m_pVertices = new Vertex[m_iNumVertex];
		m_pIndices = new Index[m_iNumIndex];

		void* pVtx = NULL;
		pMeshD->LockVertexBuffer(0, &pVtx);

		memcpy(m_pVertices, pVtx, m_iNumVertex * sizeof(Vertex));

		pMeshD->UnlockVertexBuffer();

		void* pIdx = NULL;
		pMeshD->LockIndexBuffer(0, &pIdx);

		memcpy(m_pIndices, pIdx, m_iNumIndex * sizeof(Index));

		pMeshD->UnlockIndexBuffer();

		pMeshD->Release();


		float fSCale = 10;
		for (int i = 0; i < m_iNumVertex; ++i)
		{
			m_pVertices[i].x *= fSCale;
			m_pVertices[i].y *= fSCale;
			m_pVertices[i].z *= fSCale;
		}

		m_vTiger = D3DXVECTOR3(0.0f, .0f, -20.f);
	}

	// Setup Wall
	D3DXCreateTextureFromFile(m_pdev, "Ex08_01/seafloor.bmp", &m_pWallTex);

	m_pWallVertices[0] = Vertex(-20.f, -20.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f);
	m_pWallVertices[1] = Vertex(-20.f, 20.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f);
	m_pWallVertices[2] = Vertex(0.f, 20.0f, 0.0f, 0xFFFFFFFF, 1.0f, 0.0f);
	m_pWallVertices[3] = Vertex(0.f, -20.0f, 0.0f, 0xFFFFFFFF, 1.0f, 0.0f);

	m_pMirrorVertices[0] = Vertex(0.f, -20.0f, 0.0f, 0xFFFFFFFF, 0.0f, 1.0f);
	m_pMirrorVertices[1] = Vertex(0.f, 20.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f);
	m_pMirrorVertices[2] = Vertex(20.f, 20.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f);
	m_pMirrorVertices[3] = Vertex(20.f, -20.0f, 0.0f, 0xFFFFFFFF, 0.0f, 1.0f);

	return S_OK;
}

void CBasic_3dapi_08_02::Release()
{

}

void CBasic_3dapi_08_02::Render()
{
	if (m_pdev)
	{
		D3DXMATRIX mtWorld;	D3DXMatrixIdentity(&mtWorld);
		D3DXMATRIX mtRotY;	D3DXMatrixRotationY(&mtRotY, D3DXToRadian(GetTickCount() *0.05F));
		D3DXMATRIX mtScale; D3DXMatrixScaling(&mtScale, 10, 10, 10);
		D3DXMATRIX mtIdentity;		D3DXMatrixIdentity(&mtIdentity);

		//m_pdev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_pdev->SetRenderState(D3DRS_LIGHTING, FALSE);
		m_pdev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		m_pdev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_pdev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		// 1. 스텐실이 Default로 설정되어 있는 호랑이와 벽을 먼저 그린다.
		// 호랑이
		D3DXMatrixRotationY(&mtRotY, D3DXToRadian(GetTickCount() *0.05F));
		D3DXMatrixMultiply(&mtWorld, &mtScale, &mtRotY);
		mtWorld._41 = m_vTiger.x; mtWorld._42 = m_vTiger.y; mtWorld._43 = m_vTiger.z;

		m_pdev->SetTransform(D3DTS_WORLD, &mtWorld);
		m_pdev->SetTexture(0, m_pTigerTex);

		m_pMeshS->DrawSubset(0);

		m_pdev->SetTexture(0, NULL);
		/*m_pdev->SetFVF(Vertex::FVF);
		m_pdev->SetTexture(0, m_pTxTiger);
		m_pdev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_nVtx, m_nIdx, m_pIdx, D3DFMT_INDEX16, m_pVtx, sizeof(VtxUV1));

		m_pdev->SetTexture(0, NULL);*/
		m_pdev->SetTransform(D3DTS_WORLD, &mtIdentity);

		// 왼쪽의 벽
		m_pdev->SetTexture(0, m_pWallTex);
		m_pdev->SetFVF(Vertex::FVF);
		m_pdev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_pWallVertices, sizeof(Vertex));

		// 2. 스텐실 적용을 위한 오른쪽 벽을 그린다.
		// 호랑이가 벽 뒤에 그려지기 때문에 벽면의 z값을 갱신 하지 않도록 한다.
		m_pdev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		m_pdev->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		m_pdev->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		m_pdev->SetRenderState(D3DRS_STENCILREF, 0x1);
		m_pdev->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
		m_pdev->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
		m_pdev->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		m_pdev->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		m_pdev->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

		m_pdev->SetTexture(0, m_pWallTex);
		m_pdev->SetFVF(Vertex::FVF);
		m_pdev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_pMirrorVertices, sizeof(Vertex));

		m_pdev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		// 3. 호랑이에 스텐실을 적용
		m_pdev->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		m_pdev->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		m_pdev->SetRenderState(D3DRS_STENCILREF, 0x1);
		m_pdev->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
		m_pdev->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
		m_pdev->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		m_pdev->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		m_pdev->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

		D3DXMATRIX mtTrans;
		D3DXMATRIX mtReflect;
		D3DXPLANE plane(0.0f, 0.0f, -1.0f, 0.0f); // xy plane
		D3DXMatrixReflect(&mtReflect, &plane);
		D3DXMatrixTranslation(&mtTrans, m_vTiger.x, m_vTiger.y /*+ 12*/, -m_vTiger.z);

		mtWorld = mtScale * mtRotY * mtReflect * mtTrans;

		// 반사 행렬에 의해 CW에서 CCW가 컬링 된다. 이것을 바꾸어야 한다.
		m_pdev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		
		// 호랑이를 그린다.
		m_pdev->SetTransform(D3DTS_WORLD, &mtWorld);
		/*m_pdev->SetTexture(0, m_pTigerTex);

		m_pMeshS->DrawSubset(0);*/

		m_pdev->SetFVF(Vertex::FVF);
		m_pdev->SetTexture(0, m_pTigerTex);
		m_pdev->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, m_iNumVertex, m_iNumIndex, m_pIndices, D3DFMT_INDEX16, m_pVertices, sizeof(Vertex));

		m_pdev->SetTransform(D3DTS_WORLD, &mtIdentity);
		m_pdev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		m_pdev->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		m_pdev->SetTexture(0, NULL);
	}
}

void CBasic_3dapi_08_02::Update()
{

}
