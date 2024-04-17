#pragma once
#include <EngineCore/GameMode.h>
#include "HoloCursor.h"
#include "Player.h"
#include "PlayBackGround.h"

struct FIntPoint
{
	union
	{
		struct
		{
			int X;
			int Y;
		};

		__int64 Key;
	};
};

class APlayGameMode : public AGameMode
{
	GENERATED_BODY(AGameMode)

public:
	// constrcuter destructer
	APlayGameMode();
	~APlayGameMode();

	// delete Function
	APlayGameMode(const APlayGameMode& _Other) = delete;
	APlayGameMode(APlayGameMode&& _Other) noexcept = delete;
	APlayGameMode& operator=(const APlayGameMode& _Other) = delete;
	APlayGameMode& operator=(APlayGameMode&& _Other) noexcept = delete;

protected:
	void BeginPlay() override;
	void Tick(float _DeltaTime) override;

	void LevelEnd(ULevel* _NextLevel);
	void LevelStart(ULevel* _PrevLevel);

	std::vector<std::shared_ptr<APlayBackGround>> BackGroundVector;

	std::shared_ptr<AHoloCursor> Cursor;
	std::shared_ptr<APlayer> Player;

	float4 IndexToCenterPos(FIntPoint _Index);
	FIntPoint PosToIndex(float4 _Pos);

	void InfinityGroundCheck();

	void SpawnMonster(std::string _Name, float _Size, float _Hp, float _Atk, float _Speed, float _Exp, float4 _Location);
	float4 RandomLocation();

	void PlayDebugText();

private:
	int aa;
	FIntPoint CurIndex;

	float PlayTime = 0;
	float SpawnTerm = 0;

};

