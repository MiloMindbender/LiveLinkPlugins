// Copyright 2022 by Greg Corson and Kangaroo Koncepts
// Software offered as is, no warranties
// You may use and redistribute this software as long as my copyright and "free" notice is retained.
// All redistributions and derivative works based on this software must be offered free
//

#pragma once

#include "ILiveLinkSource.h"
#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"
#include "IMessageContext.h"
#include "Interfaces/IPv4/IPv4Endpoint.h"

//enable logging step 1
DECLARE_LOG_CATEGORY_EXTERN(ModuleLog, Log, All)

class FRunnableThread;
class FSocket;
class ILiveLinkClient;
class ISocketSubsystem;


class BLISSTRACKER_API FBlissTrackerSource : public ILiveLinkSource, public FRunnable
{
public:

    FBlissTrackerSource(FIPv4Endpoint Endpoint);

    virtual ~FBlissTrackerSource();

    // Begin ILiveLinkSource Interface

    virtual void ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid) override;

    virtual bool IsSourceStillValid() const override;

    virtual bool RequestSourceShutdown() override;

    virtual FText GetSourceType() const override { return SourceType; };
    virtual FText GetSourceMachineName() const override { return SourceMachineName; }
    virtual FText GetSourceStatus() const override { return SourceStatus; }

    // End ILiveLinkSource Interface

    // Begin FRunnable Interface

    virtual bool Init() override { return true; }
    virtual uint32 Run() override;
    void Start();
    virtual void Stop() override;
    virtual void Exit() override { }

    // End FRunnable Interface

    void HandleReceivedData(TSharedPtr<TArray<uint8>, ESPMode::ThreadSafe> ReceivedData);

private:

    ILiveLinkClient* Client;

    // Our identifier in LiveLink
    FGuid SourceGuid;

    FMessageAddress ConnectionAddress;

    FText SourceType;
    FText SourceMachineName;
    FText SourceStatus;

    FIPv4Endpoint DeviceEndpoint;

    // Socket to receive data on
    FSocket* Socket;

    // Subsystem associated to Socket
    ISocketSubsystem* SocketSubsystem;

    // Threadsafe Bool for terminating the main thread loop
    FThreadSafeBool Stopping;

    // Thread to run socket operations on
    FRunnableThread* Thread;

    // Name of the sockets thread
    FString ThreadName;

    // Time to wait between attempted receives
    FTimespan WaitTime;

    // List of subjects we've already encountered
    TSet<FName> EncounteredSubjects;

    // Buffer to receive socket data into
    TArray<uint8> RecvBuffer;
};