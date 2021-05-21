#pragma once

enum Message
{
	ClientCreated,
	ClientDisconnect,
	ClientReady,
	ClientCarSelected,
	ClientNickname,
	UpdateWorld,
	Movement,
	SceneGameplay,
	RemovePlayer,
	PlayerNicknames,
	PlayerCarSelected,
	ErrorServerFull,
	Gear_change
};