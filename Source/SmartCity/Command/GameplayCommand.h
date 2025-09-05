// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class AHumanCharacter;

namespace SmartCityCommand
{
	void ReplyCameraTransform();

	static FAutoConsoleCommand ReplyCameraTransformCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("ReplyCameraTransform"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(ReplyCameraTransform),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void AdjustCameraSeat(const TArray< FString >& Args);

	static FAutoConsoleCommand AdjustCameraSeatCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("AdjustCameraSeat"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(AdjustCameraSeat),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void SwitchViewArea(const TArray< FString >& Args);

	static FAutoConsoleCommand SwitchViewAreaCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("SwitchViewArea"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(SwitchViewArea),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void SwitchMode(const TArray< FString >& Args);

	static FAutoConsoleCommand SwitchModeCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("SwitchMode"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(SwitchMode),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void SwitchInteraction(const TArray< FString >& Args);

	static FAutoConsoleCommand SwitchInteractionCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("SwitchInteraction"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(SwitchInteraction),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void TestAssetUserData();

	static FAutoConsoleCommand TestAssetUserDataCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("TestAssetUserData"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandDelegate::CreateStatic(TestAssetUserData),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void AddFeatureItem(const TArray< FString >& Args);

	static FAutoConsoleCommand AddFeatureItemCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("AddFeatureItem"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(AddFeatureItem),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void LocaterByID(const TArray< FString >& Args);

	static FAutoConsoleCommand LocaterByIDCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("LocaterByID"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(LocaterByID),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void ElevatorMoveToFloor(const TArray< FString >& Args);

	static FAutoConsoleCommand ElevatorMoveToFloorcMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("ElevatorMoveToFloor"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(ElevatorMoveToFloor),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
	void SetSpaceFeature(const TArray< FString >& Args);

	static FAutoConsoleCommand SetSpaceFeatureCMD(
		//CMD 名字，在控制台输入这个调用
		TEXT("SetSpaceFeature"),
		//控制台帮助信息，选择这个命令的时候会看到
		TEXT("this is a CMD test."),
		//创建静态委托，输入上面的命令后会调用到后面的函数
		FConsoleCommandWithArgsDelegate::CreateStatic(SetSpaceFeature),
		//可选标志位掩码
		EConsoleVariableFlags::ECVF_Default
	);
	
};
