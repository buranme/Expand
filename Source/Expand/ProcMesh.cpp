// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcMesh.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/Canvas.h"

// Sets default values
AProcMesh::AProcMesh()
{
	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProcMesh");
	RootComponent = ProcMesh;
	SectionCount = 0;
}

UMaterialInstanceDynamic* AProcMesh::InitializeMesh()
{
	FMath* MathHelper = new FMath();

	Vertices.Empty();
	Triangles.Empty();
	UVs.Empty();
	Seeds.Empty();
	srand(time(NULL));

	Height = 1025;
	Width = 1025;
	Size = 10;

	TileWidth = Width - 1;
	TileHeight = Height - 1;

	float Scale = 293.31; //Controls the scaling on the noise map
	float Fade = 0.23; //Controls the difference of Z level in between the layers
	float Zoom = 1.0 / 3.0; //Controls the difference of scaling in between the layers
	float Amplitude = Size * 10; //Controls the Z level
	float WaterLevel = 0.0; //Controls how much the landmass will sink in
	int32 Layer_Count = 5; //The amount of layers
	float UVFractionX = 1.0 / Width;
	float UVFractionY = 1.0 / Height;

	float Effective_Scale, Effective_Amplitude;

	FString GameMode = "Base"; //Base, Archipelago, Dry

	if (GameMode == "Archipelago")
	{
		Scale = 92.14;
		WaterLevel = 80;
	}

	if (GameMode == "Dry")
	{
		WaterLevel = -100;
	}

	for (int i = 0; i < Layer_Count; i++) {
		Seeds.Add(rand() % 10000);
	}

	TArray<int> TempPrecSeeds = { rand() % 10000 ,rand() % 10000 ,rand() % 10000 ,rand() % 10000 };

	CustomTexture = UTexture2D::CreateTransient(Height-1, Width-1);
	FTexture2DMipMap* MipMap = &CustomTexture->PlatformData->Mips[0];
	FByteBulkData* ImageData = &MipMap->BulkData;
	uint8* RawImageData = (uint8*)ImageData->Lock(LOCK_READ_WRITE);


	for (float i = 0.0; i < (float)Height; i += 1.0)
	{
		for (float j = 0.0; j < (float)Width; j += 1.0)
		{
			UVs.Add(FVector2D(FMath::Clamp(j*UVFractionX,0.f,1.f),FMath::Clamp(1 - (i+1)*UVFractionY, 0.f, 1.f)));

			float XValue = i * Size;
			float YValue = j * Size;

			Effective_Scale = Scale;
			Effective_Amplitude = Amplitude;
			float ZValue = (MathHelper->PerlinNoise3D(FVector(i / Effective_Scale, j / Effective_Scale, Seeds[0])) + 1) * Effective_Amplitude - WaterLevel;
			float TValue = MathHelper->PerlinNoise3D(FVector(i / Effective_Scale, j / Effective_Scale, Seeds[0]+1)) * Effective_Amplitude - WaterLevel;
			for (int k = 1; k < Layer_Count; k++) {
				Effective_Scale *= Fade;
				Effective_Amplitude *= Zoom;
				ZValue += (MathHelper->PerlinNoise3D(FVector(i / Effective_Scale, j / Effective_Scale, Seeds[k])) + 1) * Effective_Amplitude;
				TValue += MathHelper->PerlinNoise3D(FVector(i / Effective_Scale, j / Effective_Scale, Seeds[k]+1)) * Effective_Amplitude / 2;
			}
			Vertices.Add(FVector(XValue, YValue, 0));
			Normals.Add(FVector(0, 0, 1));

			if (i < (float)Height - 1.0 && j < (float)Width - 1.0)
			{
				float RandomTemperature = (MathHelper->PerlinNoise3D(FVector(i / Scale, j / Scale, TempPrecSeeds[0]))*2 + MathHelper->PerlinNoise3D(FVector(i / Scale*Fade, j / Scale*Fade, TempPrecSeeds[1])))/6 + 0.5;
				float AmplitudeTemperature = ZValue / Amplitude * 85;
				float VerticalTemperature = 1 - abs(i * 2 / Height - 1.0);
				float TempTemperature = 0.15 * RandomTemperature + 0.3 * VerticalTemperature + 0.55 * AmplitudeTemperature;

				float TempPrecipitation = TValue / Amplitude / (1 + Zoom / 2) / 2.0 + 0.5;
				TempPrecipitation = 1 - TempPrecipitation;

				RawImageData[4 * (int)((TileHeight - i - 1) * TileHeight + j)] = ZValue;
				RawImageData[4 * (int)((TileHeight - i - 1) * TileHeight + j) + 1] = 255 * TempPrecipitation;
				RawImageData[4 * (int)((TileHeight - i - 1) * TileHeight + j) + 2] = 255 * TempTemperature;
				RawImageData[4 * (int)((TileHeight - i - 1) * TileHeight + j) + 3] = 0;
			}
		}
	}

	A = 0;
	B = Width;
	C = Width - 1;
	Line_Mod = C * 2;
	Triangle_Count = (Width - 1) * (Height - 1) * 2;

	for (int i = 0; i < Triangle_Count; i++)
	{
		if (i % 2 == 0) {
			B = B - Width + 1;
			C++;
		}
		else {
			A++;
			B += Width;
		}
		if (i != 0 && i % Line_Mod == 0)
		{
			A++;
			B++;
			C++;
		}
		Triangles.Add(A);
		Triangles.Add(B);
		Triangles.Add(C);


	}


	ImageData->Unlock();
	CustomTexture->UpdateResource();
	UMaterialInstanceDynamic* DynamicMaterial = ProcMesh->CreateDynamicMaterialInstance(0, Material);
	DynamicMaterial->SetTextureParameterValue(FName("TextureParam"), CustomTexture);

	ProcMesh->CreateMeshSection(SectionCount, Vertices, Triangles, Normals, UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	//ProcMesh->CreateMeshSection(SectionCount, TArray<FVector>({ {0,0,0},{10250,0,0},{0,10250,0},{10250,10250,0} }), TArray<int32>({0,2,1,1,2,3}), TArray<FVector>({ {0,0,1},{0,0,1},{0,0,1},{0,0,1} }), TArray<FVector2D>({ {0,0},{1,0},{0,1},{1,1} }), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	//ProcMesh->SetMaterial(SectionCount, DynamicMaterial);
	SectionCount++;

	return DynamicMaterial;
}

TArray<int32> AProcMesh::GetVerticesFromVertex(int32 Point)
{
	TArray<int32> Neighbors;
	if (Point >= Width)
		Neighbors.Add(Point - Width);
	if (Point + Width < Width * Height)
		Neighbors.Add(Point + Width);
	if (Point % Width != 0)
		Neighbors.Add(Point - 1);
	if (Point % Width != Width - 1)
		Neighbors.Add(Point + 1);
	return Neighbors;
}

int32 AProcMesh::GetLowestNeighbor(int32 Point)
{
	int32 Lowest = Point;
	TArray<int32> Neighbors = GetVerticesFromVertex(Point);
	for (int32 elem : Neighbors)
	{
		if (Vertices[elem][2] < Vertices[Lowest][2])
			Lowest = elem;
	}
	if (Lowest == Point)
		return -1; //Means the point in question is a local minimum
	return Lowest;
}

TArray<int32> AProcMesh::GetVerticesFromTile(int32 Point)
{
	int32 Row = Point / TileWidth;
	return { Point + Row, Point + Row + 1, Point + Row + Width, Point + Row + Width + 1 };
}

TArray<int32> AProcMesh::GetTilesFromVertex(int32 Point)
{
	TArray<int32> Neighbors;
	int32 Row = Point / TileWidth;

	if (Point + Width < Width * Height)//Top
	{
		if (Point % Width != TileWidth)//Right
		{
			Neighbors.Add(Point - Row);
		}
		if (Point % Width != 0)//Left
		{
			Neighbors.Add(Point - Row - 1);
		}
	}
	if (Point >= Width)//Bottom
	{
		if (Point % Width != TileWidth)//Right
		{
			Neighbors.Add(Point - Width - Row);
		}
		if (Point % Width != 0)//Left
		{
			Neighbors.Add(Point - Width - Row - 1);
		}

	}
	return Neighbors;
}

TArray<int32> AProcMesh::GetTilesFromTile(int32 Point)
{
	TArray<int32> Neighbors;
	if (Point >= TileWidth)
		Neighbors.Add(Point - TileWidth);
	if (Point + TileWidth < TileWidth * (Height - 1))
		Neighbors.Add(Point + TileWidth);
	if (Point % TileWidth != 0)
		Neighbors.Add(Point - 1);
	if (Point % TileWidth != TileWidth - 1)
		Neighbors.Add(Point + 1);
	return Neighbors;
}

TArray<FVector> AProcMesh::CreateVertexArrayFromPoints(TArray<int32> Points, float Raise)
{
	TArray<FVector> VertexArray;
	FVector TempVector = Vertices[Points[0]];
	for(int i = 0; i < 4; i++)
		VertexArray.Add(FVector(Vertices[Points[i]][0], Vertices[Points[i]][1], Vertices[Points[i]][2] + Raise));
	return VertexArray;
}

void AProcMesh::CreateMeshOnTile(int32 Point)
{
	TArray<FVector> VertexArray = CreateVertexArrayFromPoints(GetVerticesFromTile(Point), 0.1);
	TArray<int32> TriangleArray = { 0,1,2,1,3,2 };
	TArray<FVector2D> UVArray = { {0,0},{0,1},{1,0},{1,1} };
	ProcMesh->CreateMeshSection(SectionCount, VertexArray, TriangleArray, TArray<FVector>(), UVArray, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	if (CityMaterial.Num() != 0)
		ProcMesh->SetMaterial(SectionCount, CityMaterial[rand() % CityMaterial.Num()]);
	SectionCount++;
	return;
}

TArray<int32> AProcMesh::InitializeRiverFromVertex(TArray<int32> River, int32 Current)
{
	River.Add(Current);
	int32 Next = GetLowestNeighbor(Current);
	//Vertices[Current][2] -= Size; //River washaway
	if (Next == -1 || Vertices[Next][2] < 0)
	{
		return River;
	}
	return InitializeRiverFromVertex(River, Next);
}

void AProcMesh::CreateMesh()
{
	ProcMesh->CreateMeshSection(SectionCount, Vertices, Triangles, TArray<FVector>(), UVs, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	if (Material)
		ProcMesh->SetMaterial(SectionCount, Material);
	SectionCount++;
}

// Called when the game starts or when spawned
void AProcMesh::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProcMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
