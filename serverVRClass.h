/*****************************************************************************
 *  serverVRClass.h
 *
 *  T-REX TEAM Unreal Engine VR Application
 *  Original Release: May 2022
 *  By: Lucas Godefroy
 *	
 *  File Description:
 *	Header file pour la class du serveur Winsock.
 * 
 *  Modification History:
 *    MAY 24 (LG): Original release
 *
 *  Copyright (c) T-REX TEAM IUT CACHAN LP MECSE
 *  All rights reserved.
 *****************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "serverVRClass.generated.h"

UCLASS()
class PROJETVR_API AserverVRClass : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AserverVRClass();

	UPROPERTY(EditAnywhere);
	int test;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	int server();
};
