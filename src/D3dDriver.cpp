
// D3dDriver.cpp
// Implementation of our Direct3d 8 rendering device

#include "Fizz3d.h"
#include "D3dDriver.h"
#include "Log.h"
#include "Engine.h"
#include "Directx8.h"
#include "Q3Structs.h"

#include "D3dDriver_TextureFactory.h"


static const char* moduleName = "D3dDriver";


// Standard constructor
D3dDriver::D3dDriver() :
	pD3dObject(NULL), pDevice(NULL)
{
}


// Standard destructor
D3dDriver::~D3dDriver()
{
	Trace("Shutting down Direct3d");

	if(pD3dObject != NULL)
	{
		pD3dObject->Release();
		pD3dObject = NULL;
	}

	if(pDevice != NULL)
	{
		pDevice->Release();
		pDevice = NULL;
	}
}


// Initialize our rendering device
int D3dDriver::initialize(IWindow* pWindow)
{
	HRESULT hr;
	
	Trace("Initializing Direct3d 8 graphics driver");

	pD3dObject = Direct3DCreate9(D3D_SDK_VERSION);

	if(pD3dObject == NULL)
		CEngine::CriticalError("Unable to create D3d8 object!");
	
	Trace("Getting display mode");
	hr = pD3dObject->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);

	if(FAILED(hr))
		CEngine::CriticalError("Unable to get adapter display mode!");

	ZeroMemory(&parameters, sizeof(parameters));
	
	if(pWindow->isFullscreen())
		parameters.Windowed = FALSE;
	else
		parameters.Windowed = TRUE;

	parameters.BackBufferFormat = displayMode.Format;
	parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;

	Trace("Creating D3d8 device");
	hr = pD3dObject->CreateDevice(
						D3DADAPTER_DEFAULT, 
						D3DDEVTYPE_HAL,
						static_cast < HWND > (pWindow->getWindowHandle()),
						D3DCREATE_SOFTWARE_VERTEXPROCESSING,
						&parameters, &pDevice);

	if(FAILED(hr))
		CEngine::CriticalError("Unable to create D3d8 device!");


	// Turn off culling, so we see the front and back of the triangle
    pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Turn off D3D lighting, since we are providing our own vertex colors
    pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	// Get color from COLORARG1
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

	// ... get color from the texture itself
	pDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);

	// Disable alpha blending
	pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

	return 0;
}


// Called at the start of each frame
int D3dDriver::startFrame(void)
{
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	pDevice->BeginScene();
	return 0;
}


// Called at the end of each frame
int D3dDriver::endFrame(void)
{
	pDevice->EndScene();
	pDevice->Present(NULL, NULL, NULL, NULL);
	return 0;
}


// Resize the viewport
int D3dDriver::resize(int newWidth, int newHeight)
{
	return 0;
}


// Sets up the camera
int D3dDriver::applyCamera(Camera* pCamera)
{
	Vec3 ref;
	D3DXMATRIX viewMatrix;
	D3DXVECTOR3 target;
	D3DXVECTOR3 eye(pCamera->getPosition().x, pCamera->getPosition().y, pCamera->getPosition().z);
	
	Vec3 cameraUp;
	pCamera->getUpVector(cameraUp);
	D3DXVECTOR3 up(cameraUp.x, cameraUp.y, cameraUp.z);

	pCamera->getReferencePoint(ref);
	target.x = ref.x;
	target.y = ref.y;
	target.z = ref.z;

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixTranslation(&matWorld, -eye.x, -eye.y, -eye.z);
	pDevice->SetTransform(D3DTS_WORLD, &matWorld);

	D3DXMatrixLookAtLH(&viewMatrix, &eye, &target, &up);
	pDevice->SetTransform(D3DTS_VIEW, &viewMatrix);

	D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 2500.0f);
    pDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	return 0;
}


// Draws the list of faces
// FIXME: use new facelist structure
int D3dDriver::drawGeometryList(GeometryList* l)
{
	HRESULT hr;
	IDirect3DVertexBuffer9* vertexBuffer;
	VOID* pVertices;

	Assert(l != NULL);

	hr = pDevice->CreateVertexBuffer(
				l->mapData->numVertices * sizeof(Q3Vertex),
				0,
				D3DVERTEXTYPE2,
				D3DPOOL_DEFAULT,
				&vertexBuffer, NULL); // fixme

	Assert(!FAILED(hr));

	vertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
	memcpy(pVertices, l->mapData->pVertex, sizeof(Q3Vertex) * l->mapData->numVertices);
	vertexBuffer->Unlock();

	pDevice->SetStreamSource(0, vertexBuffer, sizeof(D3DVERTEXTYPE2), 0); // fixme
	//pDevice->SetVertexShader(D3DVERTEXTYPE2); fixme

	for(int j = 0; j < l->faceCount; ++j)
	{
		int i = l->faces[j];

		//textureFactory->applyTexture(l->mapData->pShader[l->mapData->pFace[i].texture].textureId, 0);
		//pDevice->SetTexture(NULL, 
		//pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, l->mapData->pFace[i].vertex, l->mapData->pFace[i].n_vertexes);
		
		pDevice->DrawPrimitive(D3DPT_POINTLIST, 0, l->mapData->numVertices);
	}
	
	vertexBuffer->Release();

	/*HRESULT hr;
	
	IDirect3DVertexBuffer8* vertexBuffer;
	VOID* pVertices;

	hr = pDevice->CreateVertexBuffer(
									l->numVertices * sizeof(Vertex),
                                    0,
									D3DVERTEXTYPE,
                                    D3DPOOL_DEFAULT,
									&vertexBuffer);

	hr = vertexBuffer->Lock( 0, sizeof(Vertex) * l->numVertices, (BYTE**)&pVertices, 0 );
	memcpy(pVertices, l->vertexList, sizeof(Vertex) * l->numVertices );
	vertexBuffer->Unlock();

	pDevice->SetStreamSource(0, vertexBuffer, sizeof(D3DVERTEXTYPE));
	pDevice->SetVertexShader(D3DVERTEXTYPE);
	pDevice->DrawPrimitive(D3DPT_POINTLIST, 0, l->numVertices);

	vertexBuffer->Release();
*/
	return 0;
}


// Creates a D3dTextureFactory object
ITextureFactory* D3dDriver::createTextureFactory(void)
{
	textureFactory = new D3dTextureFactory(pDevice);
	return textureFactory;
}


// Draws a text string to the screen
int D3dDriver::drawText(int xPos, int yPos, std::string msg)
{
//	Assert(false);
	return 0;
}


// Processing we do on the map after it has been loaded
int D3dDriver::postProcessMap(Q3Map* mapData)
{
	Assert(mapData != NULL);

	return 0;
}
