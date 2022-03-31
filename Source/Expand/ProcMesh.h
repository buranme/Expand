// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProcMesh.generated.h"

UCLASS()
class EXPAND_API AProcMesh : public AActor
{
	GENERATED_BODY()

	int32 Line_Mod;
	int32 A;
	int32 B;
	int32 C;
	int32 Triangle_Count;

	/*float Scale;
	float Fade;
	float Zoom;
	float Amplitude;
	int32 Layers;*/
	int32 TileWidth;
	int32 TileHeight;
	TArray<int32> Seeds;

	int32 SectionCount;

	
public:	
	// Sets default values for this actor's properties
	AProcMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UProceduralMeshComponent* ProcMesh;

	UFUNCTION(BlueprintCallable)
		UMaterialInstanceDynamic* InitializeMesh();

	UFUNCTION(BlueprintCallable)
		void CreateMesh();

	UFUNCTION(BlueprintCallable)
		TArray<int32> GetVerticesFromVertex(int32 Point);

	UFUNCTION(BlueprintCallable)
		int32 GetLowestNeighbor(int32 Point);

	UFUNCTION(BlueprintCallable)
		TArray<int32> GetVerticesFromTile(int32 Point);

	UFUNCTION(BlueprintCallable)
		TArray<int32> GetTilesFromVertex(int32 Point);

	UFUNCTION(BlueprintCallable)
		TArray<int32> GetTilesFromTile(int32 Point);

	UFUNCTION(BlueprintCallable)
		TArray<FVector> CreateVertexArrayFromPoints(TArray<int32> Points, float Raise);

	UFUNCTION(BlueprintCallable)
		void CreateMeshOnTile(int32 Point);

	UFUNCTION(BlueprintCallable)
		TArray<int32> InitializeRiverFromVertex(TArray<int32> River, int32 Current);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMaterialInterface* Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UMaterialInterface*> CityMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<bool> IsCoastal;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<float> Slope;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<float> Altitude;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<bool> HasRiver;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<int32> Precipitation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<int32> Temperature;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<int32> SoilQuality;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<FVector> Vertices;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<int32> Triangles;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<FVector2D> UVs;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<FVector> Normals;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int32 Height;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int32 Width;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int32 Size;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UTextureRenderTarget2D* Temp_Prec;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UTexture2D* CustomTexture;

};
