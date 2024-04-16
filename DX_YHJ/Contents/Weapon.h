#pragma once
#include <EngineCore/Actor.h>

class USpriteRenderer;
class Weapon : public AActor
{
	GENERATED_BODY(AActor)
public:
	// constrcuter destructer
	Weapon();
	~Weapon();

	// delete Function
	Weapon(const Weapon& _Other) = delete;
	Weapon(Weapon&& _Other) noexcept = delete;
	Weapon& operator=(const Weapon& _Other) = delete;
	Weapon& operator=(Weapon&& _Other) noexcept = delete;

protected:

private:

};
