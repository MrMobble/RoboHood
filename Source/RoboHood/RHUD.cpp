// Fill out your copyright notice in the Description page of Project Settings.

#include "RoboHood.h"
#include "RHUD.h"

//Class Includes
#include "RCharacter.h"

ARHUD::ARHUD(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	/* You can use the FObjectFinder in C++ to reference content directly in code. Although it's advisable to avoid this and instead assign content through Blueprint child classes. */
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDCenterDotObj(TEXT("/Game/UI/UIAssets/Textures/ReticleImage"));
	CenterDotIcon = UCanvas::MakeIcon(HUDCenterDotObj.Object);

}

void ARHUD::DrawHUD()
{
	Super::DrawHUD();

	DrawCenterDot();
}

void ARHUD::DrawCenterDot()
{
	float CenterX = Canvas->ClipX / 2;
	float CenterY = Canvas->ClipY / 2;
	float CenterDotScale = 1.0f;

	ARCharacter* Pawn = Cast<ARCharacter>(GetOwningPawn());
	if (Pawn)
	{
		Canvas->SetDrawColor(255, 255, 255, 255);
		Canvas->DrawIcon(CenterDotIcon, CenterX-8, CenterY-8, CenterDotScale);
	}
}

