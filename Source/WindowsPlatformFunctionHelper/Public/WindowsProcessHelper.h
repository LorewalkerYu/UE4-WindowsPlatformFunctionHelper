// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WindowsProcessHelper.generated.h"
UENUM(BlueprintType)
enum class EProcRunningState : uint8 
{
    ERS_NULL,
    ERS_RUNNING
};
USTRUCT()
struct FRunningProc
{
    GENERATED_USTRUCT_BODY()

public:
    FRunningProc() {};
    FRunningProc(FGuid InGuid, FProcHandle InHandle) : Guid(InGuid), Handle(InHandle) {};
    bool operator==(const FRunningProc& Other)
    {
        return Other.Guid == Guid;
    }
    FGuid Guid;
    FProcHandle Handle; 

};

/**
 * 
 */
UCLASS(BlueprintType)
class WINDOWSPLATFORMFUNCTIONHELPER_API UWindowsProcessHelper : public UObject
{

	GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category  = "WindowsProcessHelper|Excutable")
        bool RequestToRunExe(const FString& ExcutablePath, const FString& CmdLineArgument, FGuid& OutGuid);

	UFUNCTION(BlueprintCallable, Category = "WindowsProcessHelper|Excutable")
        EProcRunningState IsProRunning(const FGuid& Guid);

	UFUNCTION(BlueprintCallable, Category = "WindowsProcessHelper|Excutable")
        int32 CurrentRunningProcNum();
    bool CheckingProcRunning();

	UFUNCTION(BlueprintCallable, Category = "WindowsProcessHelper|Excutable")
		void CloseProc(const FGuid& Guid);

    virtual void BeginDestroy() override;
protected:

    int32 NextIndex = 0;
    TArray<FRunningProc> RunningProcs;

	class FWorkerThread* InnerThread = nullptr;
	class FRunnableThread* RunnableThread = nullptr;
};
