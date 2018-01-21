//---------------------------------------------------------------------------------------------------
//
//Description : Texture 02
//Original File : s0v_06_tex_earth.zip
//
//---------------------------------------------------------------------------------------------------

#pragma once
class CShader_3dapi_01_09 : public CBaseClass
{
	struct Vertex
	{
		D3DXVECTOR3 p;
		D3DCOLOR color;
		float u, v;

		Vertex(){}
		Vertex(float _x, float _y, float _z, D3DCOLOR _color, float _u, float _v)
		{
			p.x = _x; p.y = _y; p.z = _z;
			color = _color;
			u = _u; v = _v;
		}

		enum { FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1) };
	};

	LPDIRECT3DVERTEXSHADER9	m_pVertexShader;
	LPDIRECT3DVERTEXDECLARATION9	m_pFVF;

	D3DXMATRIX m_mtWorld;

	int m_iVertexNum;
	Vertex*	m_pVertices;
	LPDIRECT3DTEXTURE9		m_pTex;
	
public:
	CShader_3dapi_01_09();
	~CShader_3dapi_01_09();

	virtual HRESULT Create(LPDIRECT3DDEVICE9 pdev);
	virtual void Release();

	virtual void Render();
	virtual void Update();
};

