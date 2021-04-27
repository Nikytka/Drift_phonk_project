#pragma once

enum Message
{
	ClientCreated,
	ClientDisconnect,
	ClientReady,
	ClientCarSelected,
	UpdateWorld,
	Movement,
	SceneGameplay,
	RemovePlayer
};