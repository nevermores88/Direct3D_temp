//---------------------------------------------------------------------------------------------------
//
//Description : Texture 03
//Original File : s0v_06_tex_env.zip
//
//---------------------------------------------------------------------------------------------------

#pragma once
class CShader_3dapi_01_10 : public CBaseClass
{
	struct Vertex
	{
		D3DXVECTOR3 p;
		D3DXVECTOR3 n;

		Vertex(){}
		Vertex(float _x, float _y, float _z, float _nx, float _ny, float _nz)
		{
			p.x = _x; p.y = _y; p.z = _z;
			n.x = _nx; n.y = _ny; n.z = _nz;
		}

		enum { FVF = (D3DFVF_XYZ | D3DFVF_NORMAL) };
	};

	LPDIRECT3DVERTEXSHADER9	m_pVertexShader;
	LPDIRECT3DVERTEXDECLARATION9	m_pFVF;

	LPD3DXMESH	m_pMesh;
	LPDIRECT3DTEXTURE9 m_pTex;

public:
	CShader_3dapi_01_10();
	~CShader_3dapi_01_10();

	virtual HRESULT Create(LPDIRECT3DDEVICE9 pdev);
	virtual void Release();

	virtual void Render();
	virtual void Update();
};

