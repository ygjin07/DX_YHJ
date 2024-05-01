#pragma once

enum class ERenderOrder
{
	BackGround,
	MonsterUp,
	MonsterUIUp,
	Player,
	MonsterDown,
	MonsterUIDown,
	Weapon,
	Cursor,
};

enum class ECollisionOrder
{
	Player,
	Monster,
	Weapon,
	Obstacle,
};

enum class EDir
{
	Left,
	Right,
};

enum class EPlayerDir
{
	N,
	NE,
	E,
	SE,
	S,
	SW,
	W,
	NW,
};

enum class EMonsterMoveType
{
	Follow,
	StraightToPlayer,
	StraightToUp,
	StraightToDown,
	StraightToRight,
	StraightToLeft,
	Stop,
};