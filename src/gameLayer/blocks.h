#pragma once
#include <cstdint>

struct Block
{
	enum class Type : uint16_t
	{
		Air,
		Dirt,
		GrassBlock,
		Stone,
		Grass,
		Sand,
		SandRuby,
		SandStone,
		WoodPlank,
		StoneBricks,
		Clay,
		WoodLog,
		Leaves,
		Copper,
		Iron,
		Gold,
		CopperBlock,
		IronBlock,
		GoldBlock,
		Bricks,
		Snow,
		Ice,
		RubyBlock,
		Platform,
		Workbench,
		Glass,
		Furnance,
		Painting,
		Sappling,
		SnowBlueRuby,
		BlueRubyBlock,
		Door,
		Jar,
		Table,
		Wordrobe,
		Bookshelf,
		SnowBricks,
		IceTable,
		IceWordrobe,
		IceBookshelf,
		IcePlatform,
		SandTable,
		SandWordrobe,
		SandBookshelf,
		SandPlatform,
		WoodenChest,
		IceChest,
		SandChest,
		BoneChest,
		BoneBricks,
		BoneBench,
		BoneWordrobe,
		BoneBookshelf,
		BonePlatform,

		BLOCKS_COUNT
	};

	Type type{ Type::Air };
};