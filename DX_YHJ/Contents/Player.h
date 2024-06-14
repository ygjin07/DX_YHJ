#pragma once
#include <EngineCore/Actor.h>
#include <EngineCore/StateManager.h>
#include "ContentsEnum.h"
#include "Monster.h"

class AWeapon;
class USpriteRenderer;
class APlayer : public AActor
{
	GENERATED_BODY(AActor)

public:
	static float4 PlayerColPos;
	static float4 PlayerPos;

	// constrcuter destructer
	APlayer();
	~APlayer();

	// delete Function
	APlayer(const APlayer& _Other) = delete;
	APlayer(APlayer&& _Other) noexcept = delete;
	APlayer& operator=(const APlayer& _Other) = delete;
	APlayer& operator=(APlayer&& _Other) noexcept = delete;

	UStateManager State;

	USpriteRenderer* GetRenderer()
	{
		return Renderer;
	}

	std::string GetName()
	{
		return Name;
	}

	void SetName(std::string _Name)
	{
		Name = _Name;
	}

	EPlayerDir GetPlayerDir()
	{
		return PlayerDir;
	}

	float GetAngle()
	{
		return Angle;
	}

	int GetMaxHp()
	{
		return MaxHp;
	}

	int GetHp()
	{
		return Hp;
	}

	float GetAtk()
	{
		return Atk;
	}

	float GetAtkTime()
	{
		return AtkTime;
	}

	float GetSpeed()
	{
		return Speed;
	}

	template<typename WeaponType>
	void AddWeapon(std::string _Name)
	{
		std::shared_ptr<AWeapon> Weapon = GetWorld()->SpawnActor<WeaponType>(_Name);
		MPlayerWeapons.insert({ _Name, Weapon });
	}

protected:
	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

private:
	USpriteRenderer* Renderer;
	USpriteRenderer* ShadowRenderer;
	UCollision* Collision;
	UCollision* DetactingMonsterCollision;

	USpriteRenderer* AtkDir;
	std::shared_ptr<UCamera> Camera;

	float4 Color;
	float Angle;

	std::string Name = "Kronii";
	EPlayerDir PlayerDir = EPlayerDir::E;
	int MaxHp = 100;
	int Hp = 100;
	float Atk = 30.0f;
	float CriRate = 0.05f;
	float Haste = 0.0f;
	float AtkTime = 1.0f;
	float Speed = 1.0f;
	float CalSpeed = ContentsValue::BaseSpeed * Speed;
	float LineSpeed = CalSpeed * 0.75f;
	float Exp = 0;

	std::list<AMonster*> LDetactingMonster;

	std::map<std::string, std::shared_ptr<AWeapon>> MPlayerWeapons;
	std::map<std::string, std::shared_ptr<AWeapon>>::iterator MPlayerWeaponsIter = MPlayerWeapons.begin();

	float HitDelay = 0.0f;

	void CreatePlayerAnimation(std::string _Name);

	void CalStatus();

	void CheckMouseAimMode();
	void ChangeMoveAimAtkDir();
	void ChangeMouseAimAtkDir();

	void CheckNearMonster();

	void CheckHit(float _DeltaTime);

	//State
	void StateInit();

	void Die(float _DeltaTime);

	void Idle(float _DeltaTime);

	void RunStart();
	void Run(float _DeltaTime);

	void KeyMove(float _DeltaTime, float4 _Dir, float _Speed);
	void KeyLineMove(float _DeltaTime, float4 _Dir1, float4 _Dir2);
};