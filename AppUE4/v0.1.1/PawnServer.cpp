/*****************************************************************************
 *  PawnServer.c
 *
 *  T-REX TEAM Unreal Engine VR Application
 *  version 0.1.1
 *  Original Release: June 2022
 *  By: Lucas Godefroy
 *
 *  File Description:
 *	Code source de la classe Pawn du serveur Winsock. 
 *  On utilise un Pawn car cette classe permet de recevoir des commandes
 *  du controlleur.
 *  Cette classe permet de gerer les commandes des controlleurs HTC et le serveur
 *  qui communique avec la Raspberry PI du Trex.
 *
 *  Modification History:
 *    JUN 14 (LG): v0.1   - Original release
 *	  JUN 14 (LG): v0.1.1 - Added server closing management.
 * 	  JUN 20 (LG): v0.2.0 - Added management of controller's sensitivity.
 * 
 *  Copyright (c) T-REX TEAM IUT CACHAN LP MECSE
 *  All rights reserved.
 *****************************************************************************/

#include "PawnServer.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include <stdio.h>
#include <time.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define PORT	8888
// Sets default values
APawnServer::APawnServer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
}

// Called when the game starts or when spawned
void APawnServer::BeginPlay()
{
	Super::BeginPlay();

	initiateServer();

	if (this->clientConnected == 1)
	{
		this->serverReady = 1;
	}
	else
	{
		closeServer();
	}
}

// Called every frame
void APawnServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


int APawnServer::initiateServer() {

	WSADATA wsa;
	char enable = 1;
	ULONG nonBlock = 1;

	//
	// Setting up Winsock
	//

	UE_LOG(LogTemp, Warning, TEXT(">>> TRY INIT SERVER"));
	UE_LOG(LogTemp, Warning, TEXT(">>> Initializing Winsock..."));
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		UE_LOG(LogTemp, Error, TEXT(">>> FAIlED"));
		return 1;
	}

	UE_LOG(LogTemp, Warning, TEXT(">>> SUCCESS"));
	UE_LOG(LogTemp, Warning, TEXT(">>> CREATING SOCKET..."));

	//
	// Creating the server socket
	//

	if ((this->s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		UE_LOG(LogTemp, Error, TEXT(">>> ERROR: Could not create socket"));
		WSACleanup();
		return 1;
	}

	//
	// Allowing to reuse the same socket
	//

	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(char)) < 0) {
		UE_LOG(LogTemp, Error, TEXT(">>> ERROR: Could set REUSEADDR"));
		WSACleanup();
		return 1;
	}

	UE_LOG(LogTemp, Warning, TEXT(">>> SUCCESS"));

	// Server network settings:
	this->server.sin_family = AF_INET;
	this->server.sin_addr.s_addr = INADDR_ANY;
	this->server.sin_port = htons(PORT);

	//
	// Binding with the first socket 
	//

	if (bind(this->s, (struct sockaddr*)&this->server, sizeof(this->server)) == SOCKET_ERROR)
	{
		UE_LOG(LogTemp, Error, TEXT(">>> ERROR: error while binding.\nError: %d"), WSAGetLastError());
		closesocket(this->s);
		WSACleanup();
		return 1;
	}

	UE_LOG(LogTemp, Warning, TEXT(">>> BINDING SUCCESS\n"));

	//
	// Setting the first socket in listenning mode
	//

	UE_LOG(LogTemp, Warning, TEXT(">>> CREATING LISTENNING SOCKET...\n"));

	if (listen(this->s, 3) == SOCKET_ERROR) {
		UE_LOG(LogTemp, Error, TEXT(">>> ERROR: Listen function failed.\nError: %d"), WSAGetLastError());
		closesocket(this->s);
		WSACleanup();
		return 1;
	}

	UE_LOG(LogTemp, Warning, TEXT(">>> LISTENNING SUCCESS\n"));

	//
	// Accepting incoming client connections to the server with a new reserved socket
	//

	UE_LOG(LogTemp, Warning, TEXT(">>> SERVER: Server launched.\n"));
	UE_LOG(LogTemp, Warning, TEXT(">>> SERVER: Waiting for client to connect.\n"));

	checkForClientConnection();
	//
	// Do not close the sockets since this method is aimed at initializing the server
	// Ending the server is done with another specific method
	//

	return 0;
}


int APawnServer::checkForClientConnection() {

	UE_LOG(LogTemp, Warning, TEXT(">>> SERVER: Waiting for client to connect.\n"));
	this->c = sizeof(struct sockaddr_in);
	this->clientSocket = accept(this->s, (struct sockaddr*)&this->client, &this->c);
	if (this->clientSocket == INVALID_SOCKET)
	{
		UE_LOG(LogTemp, Error, TEXT(">>> SERVER ERROR: Could not accept client connection.\nError: %d\nServer Ready: %d\nClient Connected: %d"), WSAGetLastError(), this->serverReady, this->clientConnected);
		this->clientConnected = 0;
		return 1;
	}

	//
	// Client connected to server.
	//

	this->clientConnected = 1;

	UE_LOG(LogTemp, Warning, TEXT(">>> SERVER: CLIENT CONNECTED\n"));

	return 0;
}


void APawnServer::sendLeftControllerPressed() {
	const char* sendBuffer = "$l1";

	if (send(this->clientSocket, sendBuffer, (int)strlen(sendBuffer), 0) == SOCKET_ERROR) 
	{
		UE_LOG(LogTemp, Error, TEXT(">>> SERVER: Error while sending data.\nError: %d"), WSAGetLastError());
		closesocket(this->clientSocket);
		closesocket(this->s);
		WSACleanup();
	}


	UE_LOG(LogTemp, Warning, TEXT(">>> SERVER: LEFT CONTROLLER PRESSED DATA SENT\n"));
}

void APawnServer::sendLeftControllerReleased() 
{
	const char* sendBuffer = "$l0";

	if (send(this->clientSocket, sendBuffer, (int)strlen(sendBuffer), 0) == SOCKET_ERROR) 
	{
		UE_LOG(LogTemp, Error, TEXT(">>> SERVER: Error while sending data.\nError: %d"), WSAGetLastError());
		closesocket(this->clientSocket);
		closesocket(this->s);
		WSACleanup();
	}


	UE_LOG(LogTemp, Warning, TEXT(">>> SERVER: LEFT CONTROLLER RELEASED DATA SENT\n"));
}


void APawnServer::sendRightControllerPressed() 
{
	const char* sendBuffer = "$r1";

	if (send(this->clientSocket, sendBuffer, (int)strlen(sendBuffer), 0) == SOCKET_ERROR) 
	{
		UE_LOG(LogTemp, Error, TEXT(">>> SERVER: Error while sending data.\nError: %d"), WSAGetLastError());
		closesocket(this->clientSocket);
		closesocket(this->s);
		WSACleanup();
	}

	UE_LOG(LogTemp, Warning, TEXT(">>> SERVER: RIGHT CONTROLLER PRESSED DATA SENT\n"));
}

void APawnServer::sendRightControllerReleased() 
{
	const char* sendBuffer = "$r0";

	if (send(this->clientSocket, sendBuffer, (int)strlen(sendBuffer), 0) == SOCKET_ERROR) 
	{
		UE_LOG(LogTemp, Error, TEXT(">>> SERVER: Error while sending data.\nError: %d"), WSAGetLastError());
		closesocket(this->clientSocket);
		closesocket(this->s);
		WSACleanup();
	}

	UE_LOG(LogTemp, Warning, TEXT(">>> SERVER: RIGHT CONTROLLER RELEASED DATA SENT\n"));
}

void APawnServer::closeServer() 
{
	const char sendbuf[3] = "/q"; // Quit command to Raspberry

	if (send(this->clientSocket, sendbuf, (int)strlen(sendbuf), 0) == SOCKET_ERROR) 
	{
		UE_LOG(LogTemp, Error, TEXT(">>> SERVER: Error while sending closing data.\nError: %d"), WSAGetLastError());
	}

	closesocket(this->clientSocket);
	closesocket(this->s);
	WSACleanup();

	UE_LOG(LogTemp, Warning, TEXT(">>> SERVER: Successfully closed\n"));
}

// Called to bind functionality to input
void APawnServer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("RightController", IE_Pressed, this, &APawnServer::sendRightControllerPressed);
	InputComponent->BindAction("RightController", IE_Released, this, &APawnServer::sendRightControllerReleased);
	InputComponent->BindAction("LeftController", IE_Pressed, this, &APawnServer::sendLeftControllerPressed);
	InputComponent->BindAction("LeftController", IE_Released, this, &APawnServer::sendLeftControllerReleased);
	InputComponent->BindAction("MenuController", IE_Pressed, this, &APawnServer::closeServer);
}

