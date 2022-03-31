// Fill out your copyright notice in the Description page of Project Settings.


#include "SphericalMesh.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
ASphericalMesh::ASphericalMesh()
{
	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProcMesh");
	RootComponent = ProcMesh;
	LayerCount = 5;
	MathHelper = new FMath();

}

void ASphericalMesh::CreateSphere()
{
	// TODO: Not much of a need to use C++ type randoms, use UKismetSystemLibrary/UKismetMathLibrary static functions
	// float RandomFloat = UKismetMathLibrary::RandomFloatInRange(0, 1);
	srand(time(NULL));
	Seeds.Empty();
	FaceVertices.Empty();
	FaceTriangles.Empty();

	for (int i = 0; i < LayerCount; i++) {
		Seeds.Add(rand() % 10000);
	}
	
	MountainRangeSeed = rand() % 10000;
	MountainRangeMaskSeed = rand() % 10000;

	for (int i = -1; i < 2; i += 2)
	{
		for (int j = -1; j < 2; j += 2)
		{
			for (int k = -1; k < 2; k += 2)
			{
				// TODO: You can use ? style if conditions (Debatable)
				// FVector Y(0, Radius * j, 0)
				// FVector Z(0, 0, Radius * k)
				// CreateFace(TArray<FVector>({ {Radius * i, 0, 0 }, (i*j*k) ? Y : Z, (i*j*k) ? Z : Y }));
				if ( i*j*k == -1)
					CreateFace(TArray<FVector>({ {Radius * i, 0, 0 }, { 0, Radius * j, 0 }, { 0, 0, Radius * k } }));
				else
					CreateFace(TArray<FVector>({ {Radius * i, 0, 0 }, { 0, 0, Radius * k }, { 0, Radius * j, 0 } }));
			}
		}
	}


	ProcMesh->CreateMeshSection(0, FaceVertices, FaceTriangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	ProcMesh->SetMaterial(0, Material);
}

void ASphericalMesh::CreateFace(TArray<FVector> Vertices)
{
	int32 A = FaceVertices.Num();

	for (int i = 0; i < Detail; i++)
	{
		FVector Start = ( (Detail - i) * Vertices[0] + i * Vertices[2] ) / Detail;
		FVector End = ((Detail - i) * Vertices[1] + i * Vertices[2]) / Detail;
		for (int j = 0; j <= Detail - i; j++)
		{
			FVector Temp = ((Detail - i - j) * Start + j * End) / (Detail - i);
			Temp = Radius * Temp.GetSafeNormal(0.f);


			float Effective_Scale = Scale;
			float Effective_Amplitude = Amplitude;

			FVector OnNoiseMap = (Temp + Seeds[0]) / Effective_Scale;
			
			float ZValue = MathHelper->PerlinNoise3D(OnNoiseMap) * Effective_Amplitude;
			for (int k = 1; k < LayerCount; k++) {
				Effective_Scale *= Fade;
				Effective_Amplitude *= Zoom;
				OnNoiseMap = (Temp + Seeds[k]) / Effective_Scale;
				ZValue += MathHelper->PerlinNoise3D(OnNoiseMap) * Effective_Amplitude;
			}
			// TODO: MathHelper is unnecessary. Use static functions like FMath::PerlinNoise3D(val). Always navigate to source code 
			FVector OnNoiseMountainRangeMask = (Temp + MountainRangeMaskSeed) / MountainRangeMaskScale;
			const float MountainRangeMask = MathHelper->PerlinNoise3D((OnNoiseMountainRangeMask));
			
			FVector OnNoiseMapMountain = (Temp + MountainRangeSeed) / MountainRangeScale;
			ZValue += MountainRangeMask * MountainRangeAmplitude * (1 - abs(MathHelper->PerlinNoise3D(OnNoiseMapMountain)));

			ZValue -= LandSinkIn;
			
			if(ZValue > 0)
				Temp *= ZValue + 1;
			FaceVertices.Add( Temp );
		}
	}

	FaceVertices.Add(Vertices[2]);

	for (int i = 0; i < Detail; i++)
	{
		for (int j = 0; j < Detail - i; j++)
		{
			FaceTriangles.Add(A + j);
			FaceTriangles.Add(A + 1 + j);
			FaceTriangles.Add(A + Detail + 1 - i + j);
		}
		for (int j = 0; j < Detail - i - 1; j++)
		{
			FaceTriangles.Add(A + 1 + j);
			FaceTriangles.Add(A + Detail + 2 - i + j);
			FaceTriangles.Add(A + Detail + 1 - i + j);
		}
		A += Detail + 1 - i;
	}
}

void ASphericalMesh::LoadSphere()
{
	UE_LOG(LogTemp, Warning, TEXT("MARTI"));
	ProcMesh->CreateMeshSection(0, FaceVertices, FaceTriangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	ProcMesh->SetMaterial(0, Material);
}



void ASphericalMesh::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASphericalMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}