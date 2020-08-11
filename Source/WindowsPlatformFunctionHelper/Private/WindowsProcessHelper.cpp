// Fill out your copyright notice in the Description page of Project Settings.


#include "WindowsProcessHelper.h"
#include "Utils/WorkerThread.h"

bool UWindowsProcessHelper::RequestToRunExe(const FString& ExcutablePath, const FString& CmdLineArgument, FGuid& OutGuid)
{
	const TCHAR* url = *ExcutablePath;
	FProcHandle Handle = FPlatformProcess::CreateProc(url, *CmdLineArgument, true, false, false, nullptr, -1, nullptr, nullptr);
	if (!Handle.IsValid())
	{
		return false;
	}
	FRunningProc Running(FGuid::NewGuid(), Handle);
	RunningProcs.Add(Running);
	if (RunningProcs.Num() == 1)
	{
		InnerThread = new FWorkerThread([this] { return CheckingProcRunning(); });
		RunnableThread = FRunnableThread::Create(InnerThread, TEXT("WindowsProcRunningChecker"));
	}
	OutGuid = Running.Guid;
	return true;
}

EProcRunningState UWindowsProcessHelper::IsProRunning(const FGuid& Guid)
{
	for (const FRunningProc& RunningProc : RunningProcs)
	{
		if (RunningProc.Guid == Guid)
		{
			FProcHandle Handle = RunningProc.Handle;
			return (FPlatformProcess::IsProcRunning(Handle)) ? EProcRunningState::ERS_RUNNING : EProcRunningState::ERS_NULL;
		}
	}
	return EProcRunningState::ERS_NULL;
}

int32 UWindowsProcessHelper::CurrentRunningProcNum()
{
	return RunningProcs.Num();
}

bool UWindowsProcessHelper::CheckingProcRunning()
{
	// UE_LOG(LogTemp, Error, TEXT("CheckingProcRunning"));
	TArray<FRunningProc> FinishedProc;
	for (const FRunningProc& RunningProc : RunningProcs)
	{
		FProcHandle Handle = RunningProc.Handle;
		if (!FPlatformProcess::IsProcRunning(Handle))
		{
			FinishedProc.Add(RunningProc);
		}
		
	}

	for (const FRunningProc& Proc : FinishedProc)
	{
		RunningProcs.Remove(Proc);
	}

	return RunningProcs.Num() > 0;
}

void UWindowsProcessHelper::CloseProc(const FGuid& Guid)
{
	for (FRunningProc& Proc : RunningProcs)
	{
		if (Proc.Guid == Guid)
		{
			FPlatformProcess::TerminateProc(Proc.Handle,true);
			// RunningProcs.Remove(Proc);
			break;
		}
	}
}

void UWindowsProcessHelper::BeginDestroy()
{
	if (RunnableThread)
	{
		RunnableThread->Kill(true);
		delete RunnableThread;
		RunnableThread = nullptr;
	}

	if (InnerThread)
	{
		InnerThread->Stop();
		delete InnerThread;
		InnerThread = nullptr;
	}
	
	Super::BeginDestroy();
}

