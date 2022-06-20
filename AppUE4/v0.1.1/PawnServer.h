/*****************************************************************************
 *  PawnServer.h
 *
 *  T-REX TEAM Unreal Engine VR Application
 *  version 0.1.0
 *  Original Release: June 2022
 *  By: Lucas Godefroy
 *
 *  File Description:
 *	Header file pour la classe Pawn du serveur winsock.
 *
 *  Modification History:
 *    JUN 14 (LG): Original release
 *	  JUN 20 (LG): Added management of controller's sensitivity.
 * 
 *  Copyright (c) T-REX TEAM IUT CACHAN LP MECSE
 *  All rights reserved.
 *****************************************************************************/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#include "PawnServer.generated.h"

UCLASS()
class APPVR_REV2_API APawnServer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APawnServer();

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

	// Methods for catching HTC Controller inputs:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Methods for operating the server:
	int initiateServer();

	int checkForClientConnection();

	void sendLeftControllerPressed();	
	void sendLeftControllerReleased();
	void sendRightControllerPressed();
	void sendRightControllerReleased();

	void closeServer();
};
