// Fill out your copyright notice in the Description page of Project Settings.


#include "VCSteamFunctionLibrary.h"

#include "OnlineSubsystemSteam.h"

FBlueprintSessionResult UVCSteamFunctionLibrary::SteamSessionIDToSessionResult(const FString& ID)
{
	FBlueprintSessionResult Out;



	//Out.OnlineResult.Session.SessionInfo = MakeShared<FOnlineSessionInfoSteam>(ESteamSession::LobbySession, LobbyId);

	return Out;
}