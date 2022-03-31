// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "SphericalMesh.generated.h"

UCLASS()
class EXPAND_API ASphericalMesh : public AActor
{
	GENERATED_BODY()

	int32 LayerCount;
	FMath* MathHelper;
	
public:	
	// Sets default values for this actor's properties
	ASphericalMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// TODO: Make this VisibleAnywhere
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UProceduralMeshComponent* ProcMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInterface* Material;

	UFUNCTION(BlueprintCallable)
		void CreateSphere();

	UFUNCTION(BlueprintCallable)
		void CreateFace(TArray<FVector> Vertices);

	UFUNCTION(BlueprintCallable)
		void LoadSphere();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Detail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Radius;

	UPROPERTY(BlueprintReadWrite)
		TArray<FVector> FaceVertices;

	UPROPERTY(BlueprintReadWrite)
		TArray<int32> FaceTriangles;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<int32> Seeds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Fade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Zoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Amplitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float LandSinkIn;

	UPROPERTY(BlueprintReadWrite)
		float MountainRangeSeed;

	UPROPERTY(BlueprintReadWrite)
		float MountainRangeMaskSeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MountainRangeAmplitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MountainRangeScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MountainRangeMaskScale;

};
