/*****************************************************************************
 *  server.h
 *
 *  T-REX TEAM Unreal Engine VR Application
 *  version 0.2.0
 *  Original Release: May 2022
 *  By: Lucas Godefroy
 *
 *  File Description:
 *	Header file pour la class du serveur Winsock.
 *
 *  Modification History:
 *    MAY 30 (LG): Original release
 *	  JUN 8  (LG): Fixed bugs, program works on this release.
 * 
 *  Copyright (c) T-REX TEAM IUT CACHAN LP MECSE
 *  All rights reserved.
 *****************************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#include "Server.generated.h"

UCLASS()
class APPVR_REV2_API AServer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AServer();

	UPROPERTY(EditAnywhere);

	int serverReady = 0;
	int clientConnected = 0;
	int c;
	SOCKET s;
	SOCKET clientSocket;
	struct sockaddr_in server;
	struct sockaddr_in client;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	int initiateServer();
	int checkForClientConnection();
	int sendLeftControllerData(int controllerState);
	int sendRightControllerData(int controllerState);
	int closeServer();
};
