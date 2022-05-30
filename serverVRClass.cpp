/*****************************************************************************
 *  serverVRClass.cpp
 *
 *  T-REX TEAM Unreal Engine VR Application
 *  Original Release: May 2022
 *  By: Lucas Godefroy
 *	
 *	File Description:
 *	Code Source de la classe du serveur Winsock pour la communication avec la
 *	Raspberry PI.
 *	
 *  Modification History:
 *    MAY 30 (LG): Original release
 *
 *  Copyright (c) T-REX TEAM IUT CACHAN LP MECSE
 *  All rights reserved.
 *****************************************************************************/

#include "serverVRClass.h"
#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define PORT	8888

// Sets default values
AserverVRClass::AserverVRClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AserverVRClass::BeginPlay()
{
	Super::BeginPlay();
	if (server() != 0) {
		UE_LOG(LogTemp, Error, TEXT(">>> Program failed."));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT(">>> Program successfuly running..."));
	}

}

// Called every frame
void AserverVRClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


int AserverVRClass::server()
{
	WSADATA wsa;
	SOCKET s;
	SOCKET new_socket;
	struct sockaddr_in server;
	struct sockaddr_in client;
	char enable = 1;
	const char* sendbuf = "/q";
	int c;

	UE_LOG(LogTemp, Warning, TEXT(">>> Initializing Winsock..."));

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		UE_LOG(LogTemp, Error, TEXT(">>> FAIlED"));
		return 1;
	}

	UE_LOG(LogTemp, Warning, TEXT(">>> SUCCESS"));
	UE_LOG(LogTemp, Warning, TEXT(">>> CREATING SOCKET..."));

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		UE_LOG(LogTemp, Error, TEXT(">>> ERROR: Could not create socket"));	
		WSACleanup();
		return 1;
	}
	UE_LOG(LogTemp, Warning, TEXT(">>> SOCKET CREATED"));

	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(char)) < 0) {
		UE_LOG(LogTemp, Error, TEXT(">>> ERROR: Could set REUSEADDR"));
		WSACleanup();
		return 1;
	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//Binding
	if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		UE_LOG(LogTemp, Error, TEXT(">>> ERROR: error while binding.\nError: %d"), WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return 1;
	}

	UE_LOG(LogTemp, Warning, TEXT(">>> BINDING SUCCESS"));

	//Socket in Listening mode:
	if (listen(s, 3) == SOCKET_ERROR) {
		UE_LOG(LogTemp, Error, TEXT(">>> ERROR: Listen function failed.\nError: %d"), WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return 1;
	}
	 
	UE_LOG(LogTemp, Warning, TEXT(">>> LISTENNING SUCCESS"));
	
	//Accept an incoming connection
	UE_LOG(LogTemp, Warning, TEXT(">>> SERVER: Waiting for client to connect."));
	
	c = sizeof(struct sockaddr_in);
	new_socket = accept(s, (struct sockaddr*)&client, &c);
	if (new_socket == INVALID_SOCKET)
	{
		UE_LOG(LogTemp, Error, TEXT(">>> SERVER ERROR: Could not accept client connection.\nError: %d"), WSAGetLastError());
		closesocket(s);
		WSACleanup();
		return 1;
	} 

	UE_LOG(LogTemp, Warning, TEXT(">>> SERVER: CLIENT CONNECTED"));

	if (send(new_socket, sendbuf, (int)strlen(sendbuf), 0) == SOCKET_ERROR) {
		UE_LOG(LogTemp, Error, TEXT(">>> SERVER: Error while sending data.\nError: %d"), WSAGetLastError ());
		closesocket(s);
		WSACleanup();
		return 1;
	}

	UE_LOG(LogTemp, Warning, TEXT(">>> SERVER: DATA SENT"));
	
	closesocket(s);
	WSACleanup();

	return 0;
}