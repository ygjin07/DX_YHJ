#pragma once
#include <Enginecore/Actor.h>
#include <EngineCore/SpriteRenderer.h>

// ���� :
class ATitleLogo : public AActor
{
	GENERATED_BODY(AActor)

public:
	// constrcuter destructer
	ATitleLogo();
	~ATitleLogo();

	// delete Function
	ATitleLogo(const ATitleLogo& _Other) = delete;
	ATitleLogo(ATitleLogo&& _Other) noexcept = delete;
	ATitleLogo& operator=(const ATitleLogo& _Other) = delete;
	ATitleLogo& operator=(ATitleLogo&& _Other) noexcept = delete;

protected:
	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

private:
	USpriteRenderer* Renderer = nullptr;

	float LogoMoveTime = 0;
	float4 LogoMoveDir = FVector::Down * 10.0f;
};

