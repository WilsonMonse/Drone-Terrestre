/*****************************************************************************
 *  server.cpp
 *
 *  T-REX TEAM Unreal Engine VR Application
 *  version 0.2.0
 *  Original Release: May 2022
 *  By: Lucas Godefroy
 *
 *	File Description:
 *	Code Source de la classe du serveur Winsock pour la communication avec la
 *	Raspberry PI.
 *
 *  Modification History:
 *    MAY 30 (LG): Original prototype release
 *	  JUN 2  (LG): Added methods to run the server
 *				   properly (init, sending data, closing)
 *	  JUN 8  (LG): Fixed bugs, program works on this release.
 *
 *  Copyright (c) T-REX TEAM IUT CACHAN LP MECSE
 *  All rights reserved.
 *****************************************************************************/

#include "Server.h"
#include <stdio.h>
#include <time.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define PORT	8888

// Sets default values
AServer::AServer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AServer::BeginPlay()
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
void AServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (this->serverReady == 1) {
		for (int i = 0; i < 10; i++)
		{
			sendRightControllerData(1);
		}
		this->serverReady = 0;
		closeServer();
	}
}


int AServer::initiateServer() {

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

	//
	// Setting the socket in non-blocking mode avoiding freezing the Unreal Engine Application at launch
	//

	//if (ioctlsocket(this->s, FIONBIO, &nonBlock) == SOCKET_ERROR) {
	//	UE_LOG(LogTemp, Error, TEXT(">>> ERROR: setting up Non-blocking Listen function failed.\nError: %d"), WSAGetLastError());
	//	closesocket(this->s);
	//	WSACleanup();
	//	return 1;
	//}
	
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

int AServer::checkForClientConnection() {

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

int AServer::sendLeftControllerData(int controllerState) {
	const char * buffReleased = "$l0";
	const char * buffPressed = "$l1";

	if (controllerState) {
		if (send(this->clientSocket, buffPressed, (int)strlen(buffPressed), 0) == SOCKET_ERROR) {
			UE_LOG(LogTemp, Error, TEXT(">>> SERVER: Error while sending data.\nError: %d"), WSAGetLastError());
			closesocket(this->clientSocket);
			closesocket(this->s);
			WSACleanup();
			return 1;
		}
	}
	else {
		if (send(this->clientSocket, buffReleased, (int)strlen(buffReleased), 0) == SOCKET_ERROR) {
			UE_LOG(LogTemp, Error, TEXT(">>> SERVER: Error while sending data.\nError: %d"), WSAGetLastError());
			closesocket(this->clientSocket);
			closesocket(this->s);
			WSACleanup();
			return 1;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT(">>> SERVER: LEFT CONTROLLER DATA SENT\n"));

	return 0;

}

int AServer::sendRightControllerData(int controllerState) {
	const char * buffReleased = "$r0";
	const char * buffPressed = "$r1";

	if (controllerState) {
		if (send(this->clientSocket, buffPressed, (int)strlen(buffPressed), 0) == SOCKET_ERROR) {
			UE_LOG(LogTemp, Error, TEXT(">>> SERVER: Error while sending data.\nError: %d"), WSAGetLastError());
			closesocket(this->clientSocket);
			closesocket(this->s);
			WSACleanup();
			return 1;
		}
	}
	else {
		if (send(this->clientSocket, buffReleased, (int)strlen(buffReleased), 0) == SOCKET_ERROR) {
			UE_LOG(LogTemp, Error, TEXT(">>> SERVER: Error while sending data.\nError: %d"), WSAGetLastError());
			closesocket(this->clientSocket);
			closesocket(this->s);
			WSACleanup();
			return 1;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT(">>> SERVER: RIGHT CONTROLLER DATA SENT\n"));

	return 0;
}

int AServer::closeServer() {
	const char sendbuf[3] = "/q"; // Quit command to Raspberry

	if (send(this->clientSocket, sendbuf, (int)strlen(sendbuf), 0) == SOCKET_ERROR) {
		UE_LOG(LogTemp, Error, TEXT(">>> SERVER: Error while sending closing data.\nError: %d"), WSAGetLastError());
		return 1;
	}

	closesocket(this->clientSocket);
	closesocket(this->s);
	WSACleanup();

	UE_LOG(LogTemp, Warning, TEXT(">>> SERVER: Successfully closed\n"));

	return 0;
}