// Shoot Them Up, All Rights Reserved

#include "Weapon/STUProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebughelpers.h"
#include "Kismet/GameplayStatics.h"

ASTUProjectile::ASTUProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	CollisionComponent->InitSphereRadius(5.f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	SetRootComponent(CollisionComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	MovementComponent->InitialSpeed = 2000.f;
	MovementComponent->ProjectileGravityScale = 0.f;
}

void ASTUProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	check(MovementComponent);
	check(CollisionComponent);

	MovementComponent->Velocity = ShotDirection * MovementComponent->InitialSpeed;
	CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
	CollisionComponent->OnComponentHit.AddDynamic(this, &ASTUProjectile::OnProjectileHit);
	SetLifeSpan(LifeSeconds);
}
 
void ASTUProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!GetWorld()) return;

	MovementComponent->StopMovementImmediately();

	//Make DMG
	UGameplayStatics::ApplyRadialDamage(GetWorld(), 
		DamageAmount, 
		GetActorLocation(), 
		DamageRadius, 
		UDamageType::StaticClass(), 
		{GetOwner()},
		this, 
		GetController(),
		DoFullDamage);

	DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 24, FColor::Red, false, 5.f);

	Destroy(); 
}

AController* ASTUProjectile::GetController() const
{
	const auto Pawn = Cast<APawn>(GetOwner());
	return Pawn ? Pawn->GetController() : nullptr;
}
