#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include <math.h>
#include <map>

struct AABB2;
class Image;
class Window;
class SpriteDefinition;
class BitmapFont;
class Texture3D;

struct RenderConfig
{
	Window* m_window = nullptr;
	bool	m_emissiveEnabled = false;
	bool	m_computeShaderEnabled = false;
};

enum class BlendMode
{
	ALPHA,
	ADDITIVE,
	OPAQUE,
	NONE,
	COUNT
};

enum class SamplerMode
{
	POINT_CLAMP,
	BILINEAR_WRAP,
	BILINEAR_CLAMP,
	COUNT
};

enum class RasterizerMode
{
	SOLID_CULL_NONE,
	SOLID_CULL_BACK,
	SOLID_CULL_FRONT,
	WIREFRAME_CULL_NONE,
	WIREFRAME_CULL_BACK,
	COUNT
};

enum class DepthMode
{
	DISABLED,
	ENABLED,
	COUNT
};

enum class VertexType 
{
	Vertex_PCU,
	Vertex_PCUTBN,
	Vertex_Font,
	COUNT
};

struct LightingDebug
{
	int		RenderAmbient = true;
	int		RenderDiffuse = true;
	int		RenderSpecular = true;
	int		RenderEmissive = true;
	int		UseDiffuseMap = true;
	int		UseNormalMap = true;
	int		UseSpecularMap = true;
	int		UseGlossinessMap = true;
	int		UseEmissiveMap = true;
	float	Padding[3];
};

struct LightConstants
{
	Vec3	SunDirection = Vec3(2.0f, 1.0f, -1.0f);
	float	SunIntensity = 0.85f;
	float	AmbientIntensity = 0.35f;

	Vec3	EyeWorldPosition = Vec3::ZERO;

	float	MinFallOff = 0.0f;
	float	MaxFallOff = 0.1f;
	float	MinFallOffMultiplier = 0.0f;
	float	MaxFallOffMultiplier = 1.0f;

	LightingDebug LightingDebugInfo;
};

struct BlurSample
{
	Vec2 Offset;
	float Weight;
	int Padding;
};

static const int k_blurMaxSamples = 64;

struct BlurConstants
{
	Vec2 TexelSize;
	float LerpT;
	int NumSamples;
	BlurSample Samples[k_blurMaxSamples];
};
static const int k_blurConstantsSlot = 5;

class SpriteSheet
{
public:
	explicit						SpriteSheet(Texture const& texture, IntVec2 const& simpleGridLayout);

	Texture const&					GetTexture() const;
	int								GetNumSprites() const;
	SpriteDefinition const&			GetSpriteDef(int spriteIndex) const;
	void							GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex) const;
	AABB2							GetSpriteUVs(int spriteIndex) const;
	AABB2							GetSpriteUVsByIntPos(IntVec2 pos) const;

protected:
	Texture const&					m_texture;
	IntVec2							m_dimensions;
	std::vector<SpriteDefinition>	m_spritedefs;
};


class SpriteDefinition 
{
public:
	explicit						SpriteDefinition( SpriteSheet const& spriteSheet, int spriteIndex, Vec2 const& uvAtMins, Vec2 const& uvAtMaxs);
	void							GetUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs) const;
	AABB2							GetUVs() const;
	SpriteSheet const&				GetSpriteSheet() const;
	Texture const&					GetTexture() const;
	float							GetAspect() const;
protected:
	SpriteSheet const&				m_spriteSheet;
	int								m_spriteIndex = -1;
	Vec2							m_uvAtMins = Vec2::ZERO;
	Vec2							m_uvAtMaxs = Vec2::ONE;
};

//	Safe release macro to check whether or not the dxObject is a nullptr for releasing

#define DX_SAFE_RELEASE(dxObject)							\
{															\
	if ((dxObject) != nullptr)								\
	{														\
		(dxObject)->Release();								\
		(dxObject) = nullptr;								\
	}														\
}

struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11VertexShader;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11BlendState;
struct ID3D11Buffer;
struct ID3D11SamplerState;

class Shader;
class ComputeShader;
class VertexBuffer;
class ConstantBuffer;
class IndexBuffer;

class Renderer 
{
public:

	Renderer();
	Renderer(RenderConfig const& config);

	void				Startup();
	void				BeginFrame();
	void				EndFrame();
	void				Shutdown();

	void				ClearScreen(const Rgba8& clearColor);
	void				BeginCamera(const Camera& camera, bool extraInformation = false);
	void				EndCamera(const Camera& camera);
	void				BindTexture(const Texture* texture);
	void				BindTexture(const Texture* texture, unsigned int slotNum);
	void				DrawVertexArray(int numVertexes, const Vertex_PCU* vertexes);
	void				DrawVertexArray(int numVertexes, const Vertex_PCU* vertexes, int numIndexes, const unsigned int* indexes);
	void				DrawVertexArray(int numVertexes, const Vertex_PCUTBN* vertexes);
	void				DrawVertexArray(int numVertexes, const Vertex_PCUTBN* vertexes, int numIndexes, const unsigned int* indexes);
	void				DrawVertexArray( int numVertexes, Vertex_Font const* vertexArray );
	void				DrawVertexBuffer(VertexBuffer* vbo,int vertexCount, int vertexOffset = 0 , VertexType vertType = VertexType::Vertex_PCU);
	void				DrawVertexBufferAndIndexBuffer(VertexBuffer* vbo, IndexBuffer* ibo, int indexCount, int vertexOffset = 0, VertexType vertType = VertexType::Vertex_PCU);
	void				DrawVertexBufferAndIndexBufferInstanced(VertexBuffer* vbo, IndexBuffer* ibo, int indexCountPerInstance, int instanceNum);
	void				RenderEmissive();

	//---------------------------------------------------------------------------------
	// Compute shader calculation period
	void				BeginComputeShader(ComputeShader* cs, IntVec3 const& dimension);

	BitmapFont*			CreateOrGetBitmapFont(char const* pathWithoutExtension);
	BitmapFont*			CreateCustomBitmapFont(char const* xmlPath, char const* imagePath);
	BitmapFont*			GetBitMapFontFromFileName(char const* pathWithoutExtension);
	BitmapFont*			CreateFontFromFile(char const* pathWithoutExtension);

	ID3D11Buffer*		CreateCSSRVBuffer(void* data, int dataWidth, int dataNum);
	ID3D11ShaderResourceView* CreateCSSRV(ID3D11Buffer* buffer, int elementNum);

	Texture*			CreateOrGetTextureFromFile(char const* imageFilePath);
	Texture*			GetTextureForFileName(char const* imageFilePath);
	Texture*			CreateTextureFromFile(char const* imageFilePath);
	Texture*			CreateTextureFromImage(const Image& image);
	Texture*			CreateTextureFromData(char const* name, IntVec2 dimensions, int bytesPerTexel, const void* texelData);
	Texture3D*			CreateOrGetTexture3D(const std::string& textureName, int textureWidth, int textureHeight, int textureDepth);

	RenderConfig		const& GetConfig() const;

	//------------------------------------------------------------------------------------------------------
	Shader*				CreateOrGetShader(char const* shaderName, VertexType vertexType = VertexType::Vertex_PCU);
	Shader*				CreateShader(char const* shaderName, char const* sourceShader, VertexType vertexType = VertexType::Vertex_PCU);
	ComputeShader*		CreateOrGetComputeShader(const std::string& shaderName, const std::string& entryPoint = "CSMain");
	ComputeShader*		CreateComputeShader(const std::string& shaderName, const std::string& entryPoint = "CSMain");
	bool				CompileShaderToByteCode(std::vector<unsigned char>& out_ByteCode, char const* name, 
						char const* source, char const* entryPoint, char const* target);
	void				BindShader(Shader* shader,VertexType vertexType = VertexType::Vertex_PCU);

	//------------------------------------------------------------------------------------------------------
	VertexBuffer*		CreateVertexBuffer(const size_t size);
	VertexBuffer*		CreateShaderResourceViewBuffer(const size_t sizeOfElement, const size_t elementNum, void* data);

	IndexBuffer*		CreateIndexBuffer(const size_t size);
	void				CopyCPUToGPU(const void* data, size_t size, VertexBuffer*& vbo);
	void				CopyCPUToGPU(const void* data, size_t size, VertexBuffer*& vbo, const void* indexData , size_t sizeIndex, IndexBuffer*& ibo);
	
	//------------------------------------------------------------------------------------------------------
	// Bind buffers here
	void				BindVertexBuffer(VertexBuffer* vbo);
	void				BindVertexBuffer(VertexBuffer* vbo, int vertSize);
	void				BindVertexBuffer(VertexBuffer* vbo, int vertSize, int slotNum);
	void				BindVertexBuffers(std::vector<VertexBuffer*> vbos, std::vector<unsigned int> strides);
	void				BindShaderResourceView(ID3D11ShaderResourceView* vb, int slotNum);
	void				UpdateShaderResourceViewBuffer(ID3D11Buffer* vb, const void* data, size_t size);
	void				GetImmediateContext();

	void				BindIndexBuffer(IndexBuffer* ibo);
	void				BindConstantBuffer(int slot, ConstantBuffer* cbo);
	void				BindCSConstantBuffer(int slot, ConstantBuffer* cbo);
	void				BeginBindCSDepthTexture(ID3D11ShaderResourceView* srv, unsigned int slotNum);
	void				EndBindCSDepthTexture(unsigned int slotNum);
	//------------------------------------------------------------------------------------------------------
	ConstantBuffer*		CreateConstantBuffer(const size_t size);
	void				CopyCPUToGPU(const void* data, size_t size, ConstantBuffer* cbo);

	// Blend mode
	//------------------------------------------------------------------------------------------------------
	void				SetBlendMode(BlendMode blendMode);
	void				SetDepthMode(DepthMode depthMode);
	void				SetStatesIfChanged();

	//------------------------------------------------------------------------------------------------------
	void				SetSamplerMode(SamplerMode samplerMode);

	//------------------------------------------------------------------------------------------------------
	void				SetRasterizerState(RasterizerMode rasterizerMode);
	void				CreateRasterizerStateBaseOnMode();

	//------------------------------------------------------------------------------------------------------
	void				CreateBloomRenderTexture();
	void				CreateDepthStencilTextureandView();
	void				SetModelConstants(const Mat44& modelMatrix = Mat44(), const Rgba8& modelColor = Rgba8::WHITE);
	void				SetLightConstants(Vec3 sunDirection= Vec3(2.0f, 1.0f, -1.0f), float sunIntensity= 0.85f, float ambientIntensity = 0.35f);
	void				SetLightConstants(LightConstants const& lightConstants);

	ID3D11Device*		GetRendererDevice() { return m_device; }
	ID3D11DeviceContext* GetRendererDeviceContext() { return m_deviceContext; }
	ID3D11Texture2D*	GetCSDepthTexture() { return m_depthStencilTexture;}
	ID3D11RenderTargetView* GetBasicRTV() { return m_renderTargetView;}
	ID3D11DepthStencilView* GetDepthStencil() { return m_depthStencilView;}
	IDXGISwapChain*		GetSwapChain() {return m_swapchain;}
protected:

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	IDXGISwapChain* m_swapchain = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11InputLayout* m_inputLayoutForVertex_PCU = nullptr;
	void* m_dxgiDebugodule = nullptr;
	void* m_dxgiDebug = nullptr;

	// Shader cache to contain all the shaders needed
	std::vector<Shader*> m_loadedShaders;
	std::vector<ComputeShader*> m_loadedComputeShaders;
	Shader* m_currentShader = nullptr;
	Shader*	m_defaultShader = nullptr;
	ComputeShader* m_defaultComputeShader = nullptr;
	VertexBuffer* m_immediateVBO = nullptr;
	IndexBuffer* m_indexBuffer = nullptr;
	ConstantBuffer* m_cameraCBO = nullptr;
	ConstantBuffer*	m_modelCBO = nullptr;
	ConstantBuffer*	m_lightCBO = nullptr;
	ConstantBuffer*	m_blurCBO = nullptr;

	// Blend state member variable
	ID3D11BlendState* m_blendState = nullptr;
	BlendMode m_desiredBlendMode = BlendMode::ALPHA;
	ID3D11BlendState* m_blendStates[(int)(BlendMode::COUNT)] = {};

	// Sampler state variable
	ID3D11SamplerState* m_samplerState = nullptr;
	SamplerMode m_desiredSamplerMode = SamplerMode::POINT_CLAMP;
	ID3D11SamplerState* m_samplerStates[(int)SamplerMode::COUNT] = {};

	// Rasterizer state variable
	RasterizerMode m_rasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	ID3D11RasterizerState* m_desiredRasterizerState = nullptr;
	ID3D11RasterizerState* m_rasterizedStates[(int)RasterizerMode::COUNT] = {};

	// Depth enabled mode
	DepthMode m_depthMode = DepthMode::ENABLED;
	ID3D11DepthStencilView*	m_depthStencilView = nullptr;
	ID3D11Texture2D* m_depthStencilTexture = nullptr;
	ID3D11DepthStencilState* m_depthStencilState = nullptr;
	ID3D11DepthStencilState* m_depthStencilStates[(int)DepthMode::COUNT] = {};
	// Texture variables
	const Texture* m_defaultTexture = nullptr;

	// Bloom Texture
	Texture*					m_emissiveRenderTexture = nullptr;
	Texture*					m_emissiveBlurredRenderTexture = nullptr;
	std::vector<Texture*>		m_blurUpRenderTextures;
	std::vector<Texture*>		m_blurDownRenderTextures;
private:

	// Private data members here
	RenderConfig				m_config;
	void*						m_rc = nullptr;
	std::vector<Texture*>		m_loadedTextures;
	std::vector<Texture3D*>		m_loadedTexture3D;
	std::vector<BitmapFont*>	m_loadedFonts;
};

class HashedCaseInsensitiveString;
class NonUniformSpriteDefinition;

class NonUniformSpriteSheet
{
public:
	explicit						NonUniformSpriteSheet(Renderer* renderer, std::string const& xmlFilePath);

	Texture*						m_texture = nullptr;
	IntVec2							m_dimension;
	std::map<HashedCaseInsensitiveString, AABB2> m_spritedefs;
};
