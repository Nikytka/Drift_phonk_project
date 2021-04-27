#pragma once

enum Message
{
	ClientCreated,
	ClientDisconnect,
	ClientReady,
	ClientHeroSelected,
	UpdateWorld,
	Movement,
	SceneGameplay,
	RemovePlayer
};