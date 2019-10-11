#include "ShadowMapping.h"
#include"..//game.h"


ShadowMapping::ShadowMapping()
{
}


ShadowMapping::~ShadowMapping()
{
}

bool ShadowMapping::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;

	std::wstring shaderfolder = L"";
#pragma region DetermineShaderPath
	
#ifdef _DEBUG //Debug Mode
#ifdef _WIN64 //x64
		shaderfolder = L"..\\x64\\Debug\\";
#else  //x86 (Win32)
		shaderfolder = L"..\\Debug\\";
#endif
#else //Release Mode
#ifdef _WIN64 //x64
		shaderfolder = L"..\\x64\\Release\\";
#else  //x86 (Win32)
		shaderfolder = L"..\\Release\\";
#endif
#endif
	


	std::wstring filePath = shaderfolder + L"SimpleVS.cso";

	HRESULT hr = D3DReadFileToBlob(filePath.c_str(), this->vertexShaderBlob.GetAddressOf());
	hr = device->CreateVertexShader(this->vertexShaderBlob->GetBufferPointer(), this->vertexShaderBlob->GetBufferSize(), NULL, this->simpleVertexShader.GetAddressOf());
	//hr = device->CreateInputLayout(inputDesc, numElements, this->vertexShaderBlob->GetBufferPointer(), this->vertexShaderBlob->GetBufferSize(), this->inputLayout.GetAddressOf());

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{
			"SV_POSITION",		// "semantic" name in shader
			0,				// "semantic" index (not used)
			DXGI_FORMAT_R32G32B32_FLOAT, // size of ONE element (3 floats)
			0,							 // input slot
			D3D11_APPEND_ALIGNED_ELEMENT, // offset of first element
			D3D11_INPUT_PER_VERTEX_DATA, // specify data PER vertex
			0							 // used for INSTANCING (ignore)
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT, //2 values
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"NORMAL",
			0,				// same slot as previous (same vertexBuffer)
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,							// offset of FIRST element (after POSITION)
			D3D11_INPUT_PER_VERTEX_DATA,
			0
			//for normal mapping. tangent binormal
		}



	};

	

	D3D11_BUFFER_DESC descB = { 0 };

	
	descB.Usage = D3D11_USAGE_DYNAMIC;
	descB.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	descB.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	descB.MiscFlags = 0;
	descB.ByteWidth = static_cast<UINT>(sizeof(Matrix) + (16 - (sizeof(Matrix) % 16)));
	descB.StructureByteStride = 0;

	hr = device->CreateBuffer(&descB, 0, this->perFrameCB.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}
	hr = device->CreateBuffer(&descB, 0, this->worldBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}


	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = 2000/*System::getWindowArea().width*/;
	depthStencilDesc.Height = 2000/*System::getWindowArea().height*/;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	this->vp.Width = (float)depthStencilDesc.Width/*System::getWindowArea().width*/;
	this->vp.Height = (float)depthStencilDesc.Height/*System::getWindowArea().height*/;
	this->vp.MinDepth = 0.0f;
	this->vp.MaxDepth = 1.0f;
	this->vp.TopLeftX = 0;
	this->vp.TopLeftY = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	dsv_desc.Flags = 0;
	dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC sr_desc;
	sr_desc.Format = DXGI_FORMAT_R32_FLOAT;
	sr_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sr_desc.Texture2D.MostDetailedMip = 0;
	sr_desc.Texture2D.MipLevels = 1;

	if (FAILED(this->device->CreateTexture2D(&depthStencilDesc, 0, depthTexture.GetAddressOf())))
		return false;
	if (FAILED(this->device->CreateDepthStencilView(depthTexture.Get(), &dsv_desc, depthStencilView.GetAddressOf())))
		return false;
	if (FAILED(this->device->CreateShaderResourceView(depthTexture.Get(), &sr_desc, depthShaderResource.GetAddressOf())))
		return false;

	D3D11_SAMPLER_DESC desc;
	float bColor[4] = {
		1.0f, 1.0f, 1.0f, 1.0f
	};
	ZeroMemory(&desc, sizeof(desc));
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.BorderColor[0] = bColor[0];
	desc.BorderColor[1] = bColor[1];
	desc.BorderColor[2] = bColor[2];
	desc.BorderColor[3] = bColor[3];
	desc.MinLOD = 0;
	desc.MaxLOD = 1;
	hr = this->device->CreateSamplerState(&desc, this->sampler.GetAddressOf());
	if (FAILED(hr))
	{
		//MessageBox(hwnd, "Error compiling shader.  Check shader-error.txt for message.", "error", MB_OK);
		//deal with error. Log it maybe

	}
	return true;
}

void ShadowMapping::setWorld(const Matrix& world)
{
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Matrix worldTemp = world;
	HRESULT hr = deviceContext->Map(worldBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &worldTemp, sizeof(Matrix));
	deviceContext->Unmap(worldBuffer.Get(), 0);
	deviceContext->VSSetConstantBuffers(1, 1, this->worldBuffer.GetAddressOf());

}

void ShadowMapping::setViewProj(DynamicCamera *camera, Vector3 sunDir)
{
	Matrix rotationRes;
	Vector3 sunDirection = sunDir;
	sunDirection.x = 0.5f;
	sunDirection.Normalize();
	float pitch = sqrt(pow(sunDirection.x, 2) + pow(sunDirection.y, 2)) / sunDirection.z;
	pitch = atan(pitch);
	float yaw = sunDirection.x / -sunDirection.y;
	yaw = atan(yaw);
	//yaw = sunDirection.z > 0 ? yaw - (180* (DirectX::XM_PI/180)) : yaw;
	//rotationRes = Matrix::CreateRotationZ(this->rotation.z); //Roll
	rotationRes = Matrix::CreateRotationX(pitch); //Pitch
	rotationRes = Matrix::CreateFromYawPitchRoll(yaw,pitch,0.0f); //Yaw
	
	//Vector3 pos(camera->getPosition().x, camera->getPosition().y*3, camera->getPosition().z);
	Vector3 offset = (camera->getPosition().y / sunDir.y) * sunDir;
	Vector3 pos(camera->getPosition().x + offset.x, camera->getPosition().y+5, camera->getPosition().z + offset.z);
	//pos.z += -2.0f;
	//pos.y += 10.0f;
	//pos *= -1.0f;
	this->view = Matrix::CreateLookAt(
		pos, //Position in world
		pos + Vector3::Transform(Vector3(0.0f, 0.0f, -1.0f),rotationRes), //Front
		Vector3::Transform(Vector3(0.0f, 1.0f, 0.0f), rotationRes) //Up
		);
	
	this->view = Matrix::CreateLookAt(
		pos, //Position in world
		pos + sunDirection,/*pos + Vector3::Transform(Vector3(0.0f, 0.0f, -1.0f),rotationRes*/ //Front
		Vector3::Reflect(sunDirection, Vector3(0, 1, 0))/*Vector3::Transform(Vector3(0.0f, 1.0f, 0.0f), rotationRes)*/ //Up
	);

	

	/*Fr�n https://www.gamedev.net/forums/topic/505893-orthographic-projection-for-shadow-mapping/ och ska testa f�r o se om det funkar*/

	Vector3 centroid(0, 0, 0);
	Matrix invView = camera->getViewMatrix().Invert();
	Vector4 viewDir = DirectX::XMVector4Transform(Vector4(0.0, 0.0, 1.0, 0.0), invView);
	Vector4 viewUp = DirectX::XMVector4Transform(Vector4(0.0, 1.0, 0.0, 0.0), invView);
	Vector4 viewRight = DirectX::XMVector4Transform(Vector4(1.0, 0.0, 0.0, 0.0), invView);
	
	
	Vector3 nearCenter = camera->getPosition() + viewDir * camera->getNearDistance();
	Vector3 farCenter = camera->getPosition() + viewDir * camera->getFarDistance();
	Vector3 nearTopLeft = nearCenter + Vector3(viewUp) * camera->getNearHeight() - Vector3(viewRight) * camera->getNearWidth();
	Vector3 nearTopRight = nearCenter + Vector3(viewUp) * camera->getNearHeight() + Vector3(viewRight) * camera->getNearWidth();
	Vector3 nearBottomLeft = nearCenter - Vector3(viewUp) * camera->getNearHeight() - Vector3(viewRight) * camera->getNearWidth();
	Vector3 nearBottomRight = nearCenter - Vector3(viewUp) * camera->getNearHeight() + Vector3(viewRight) * camera->getNearWidth();

	Vector3 farTopLeft = farCenter + Vector3(viewUp) * camera->getFarHeight() - Vector3(viewRight) * camera->getFarWidth();
	Vector3 farTopRight = farCenter + Vector3(viewUp) * camera->getFarHeight() + Vector3(viewRight) * camera->getFarWidth();
	Vector3 farBottomLeft = farCenter - Vector3(viewUp) * camera->getFarHeight() - Vector3(viewRight) * camera->getFarWidth();
	Vector3 farBottomRight = farCenter - Vector3(viewUp) * camera->getFarHeight() + Vector3(viewRight) * camera->getFarWidth();

	centroid += nearTopLeft; centroid += nearTopRight; centroid += nearBottomLeft; centroid += nearBottomRight;
	centroid += farTopLeft; centroid += farTopRight; centroid += farBottomLeft; centroid += farBottomRight;

	centroid /= 8;

	float nearClipOffset = 50.0f;
	float farZ = 40.0f;
	float distFromCentroid = farZ + nearClipOffset;
	Matrix viewMatrix = Matrix::CreateLookAt(centroid - (sunDir * distFromCentroid), centroid, Vector3(0, 1, 0));

	Vector3 lightFCs[8];

	lightFCs[0] = Vector3::Transform(nearTopLeft,viewMatrix);
	lightFCs[1] = Vector3::Transform(nearTopRight, viewMatrix); 
	lightFCs[2] = Vector3::Transform(nearBottomLeft, viewMatrix); 
	lightFCs[3] = Vector3::Transform(nearBottomRight, viewMatrix); 
	lightFCs[4] = Vector3::Transform(farTopLeft, viewMatrix); 
	lightFCs[5] = Vector3::Transform(farTopRight, viewMatrix); 
	lightFCs[6] = Vector3::Transform(farBottomLeft, viewMatrix);
	lightFCs[7] = Vector3::Transform(farBottomRight, viewMatrix);
	
	



	/*sunDirection.Normalize();
	pos *= -1.0f;
	Matrix lightViewMatrix;
	lightViewMatrix = Matrix::Identity;
	

	lightViewMatrix.CreateFromYawPitchRoll(pitch, new Vector3f(1, 0, 0), lightViewMatrix, lightViewMatrix);
	Matrix4f.rotate((float)-Math.toRadians(yaw), new Vector3f(0, 1, 0), lightViewMatrix,
		lightViewMatrix);
	Matrix4f.translate(pos, lightViewMatrix, lightViewMatrix);*/
	
	
	//pos += camera->getPosition();
	//view = Matrix::CreateLookAt(pos, Vector3(-pos.x,0.0f,0.0f), Vector3(0.0f, 1.0f, 0.0f));
	orthoProj = Matrix::CreateOrthographic(100.0f, 100.0f, 0.01f, 25.0f);
	orthoProj = DirectX::XMMatrixOrthographicLH(200.0f, 200.0f, 1.0f, 400.0f);
	float fieldOfView = 10 * (DirectX::XM_PI / 180);
	//orthoProj = camera->getProjectionMatrix();
	this->view = viewMatrix;
	//orthoProj = Matrix::CreatePerspectiveFieldOfView(fieldOfView, 16.f / 9.f, 900, 1400.0f);
	Matrix viewProj = view * orthoProj;
	viewProj = viewProj.Transpose();
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(perFrameCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &viewProj, sizeof(Matrix));
	deviceContext->Unmap(perFrameCB.Get(), 0);
	deviceContext->VSSetConstantBuffers(0, 1, this->perFrameCB.GetAddressOf());
}


void ShadowMapping::prepare()
{
	ID3D11ShaderResourceView* fs = NULL;
	Game::getGraphics().getDeviceContext()->PSSetShaderResources(4, 1, &fs);
	Game::getGraphics().getDeviceContext()->RSSetViewports(1, &this->vp);
	
	deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	ID3D11ShaderResourceView* s = NULL;
	deviceContext->PSSetShaderResources(3, 1, &s);
	deviceContext->OMSetRenderTargets(0, nullptr, this->depthStencilView.Get());
	deviceContext->VSSetShader(this->simpleVertexShader.Get(), nullptr, 0);
	//System::getDeviceContext()->PSSetSamplers(0, 1, &this->sampler);
	//System::getDeviceContext().omset
}

Microsoft::WRL::ComPtr <ID3D11SamplerState> ShadowMapping::getShadowSampler()
{
	return this->sampler;
}


Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> ShadowMapping::getShadowMap()
{
	return this->depthShaderResource;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> ShadowMapping::getViewProj()
{
	return this->perFrameCB;
}

