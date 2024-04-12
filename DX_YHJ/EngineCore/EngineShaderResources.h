#pragma once
#include "EngineEnums.h"
#include "EngineConstantBuffer.h"
#include <EngineBase/NameObject.h>

class USetterBase : public UNameObject
{
public:
	EShaderType Type = EShaderType::NONE;
	int Slot = -1; // b0, b1, b2
};

// setter��� �༮�� ��� ���� �̴ϴ�.
class UEngineConstantBufferSetter : public USetterBase
{
public:
	std::shared_ptr<class UEngineConstantBuffer> Res;
	// �� ���͸��� ������ �ִ� �����͸� ������ �༮
	const void* CPUData = nullptr;
	UINT BufferSize = 0;

	void Setting();
};

class UEngineTextureSetter : public USetterBase
{
public:
	std::shared_ptr<class UEngineTexture> Res;
	void Setting();
};

class UEngineSamplerSetter : public USetterBase
{
public:
	std::shared_ptr<class UEngineSampler> Res;
	void Setting();
};


// ���� :
class URenderer;
class UEngineShader;
class UEngineShaderResources
{
	friend UEngineShader;
	friend URenderer;

public:
	template<typename Value>
	void SettingConstantBuffer(std::string_view _Name, Value& _Data)
	{
		SettingConstantBuffer(_Name, &_Data, static_cast<UINT>(sizeof(Value)));
	}
	bool IsConstantBuffer(std::string_view _Name);

	void SettingConstantBuffer(std::string_view _Name, const void* _Data, UINT _Size);

	void SettingTexture(std::string_view _TexName, std::string_view _ImageName, std::string_view _SamperName);

	void SettingTexture(std::string_view _TexName, std::shared_ptr<UEngineTexture> _Texture, std::string_view _SamperName);


	void SettingAllShaderResources();

	void Reset();

protected:

private:
	void ShaderResourcesCheck(EShaderType _Type, std::string_view _EntryName, ID3DBlob* _ShaderCode);

	// �ȼ����̴��� 
	// ���ؽ� ���̴� 
	std::map<EShaderType, std::map<std::string, UEngineConstantBufferSetter>> ConstantBuffers;
	std::map<EShaderType, std::map<std::string, UEngineTextureSetter>> Textures;
	std::map<EShaderType, std::map<std::string, UEngineSamplerSetter>> Samplers;

};
