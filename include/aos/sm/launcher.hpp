/*
 * Copyright (C) 2023 Renesas Electronics Corporation.
 * Copyright (C) 2023 EPAM Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef AOS_LAUNCHER_HPP_
#define AOS_LAUNCHER_HPP_

#include <assert.h>

#include "aos/common/cloudprotocol/envvars.hpp"
#include "aos/common/connectionsubsc.hpp"
#include "aos/common/monitoring/monitoring.hpp"
#include "aos/common/ocispec.hpp"
#include "aos/common/tools/map.hpp"
#include "aos/common/tools/noncopyable.hpp"
#include "aos/common/types.hpp"
#include "aos/sm/config.hpp"
#include "aos/sm/instance.hpp"
#include "aos/sm/runner.hpp"
#include "aos/sm/service.hpp"
#include "aos/sm/servicemanager.hpp"

namespace aos {
namespace sm {
namespace launcher {

/** @addtogroup sm Service Manager
 *  @{
 */

/**
 * Instance launcher interface.
 */
class LauncherItf {
public:
    /**
     * Runs specified instances.
     *
     * @param instances instance info array.
     * @param forceRestart forces restart already started instance.
     * @return Error.
     */
    virtual Error RunInstances(const Array<ServiceInfo>& services, const Array<LayerInfo>& layers,
        const Array<InstanceInfo>& instances, bool forceRestart)
        = 0;

    /**
     * Overrides environment variables for specified instances.
     *
     * @param envVarsInfo environment variables info.
     * @param statuses[out] environment variables statuses.
     * @return Error
     */
    virtual Error OverrideEnvVars(const Array<cloudprotocol::EnvVarsInstanceInfo>& envVarsInfo,
        cloudprotocol::EnvVarsInstanceStatusArray&                                 statuses)
        = 0;

    /**
     * Sets cloud connection status.
     *
     * @param connected cloud connection status.
     * @return Error
     */
    virtual Error SetCloudConnection(bool connected) = 0;
};

/**
 * Interface to send instances run status.
 */
class InstanceStatusReceiverItf {
public:
    /**
     * Sends instances run status.
     *
     * @param instances instances status array.
     * @return Error.
     */
    virtual Error InstancesRunStatus(const Array<InstanceStatus>& instances) = 0;

    /**
     * Sends instances update status.
     * @param instances instances status array.
     *
     * @return Error.
     */
    virtual Error InstancesUpdateStatus(const Array<InstanceStatus>& instances) = 0;
};

/**
 * Launcher storage interface.
 */
class StorageItf {
public:
    /**
     * Adds new instance to storage.
     *
     * @param instance instance to add.
     * @return Error.
     */
    virtual Error AddInstance(const InstanceInfo& instance) = 0;

    /**
     * Updates previously stored instance.
     *
     * @param instance instance to update.
     * @return Error.
     */
    virtual Error UpdateInstance(const InstanceInfo& instance) = 0;

    /**
     * Removes previously stored instance.
     *
     * @param instanceID instance ID to remove.
     * @return Error.
     */
    virtual Error RemoveInstance(const InstanceIdent& instanceIdent) = 0;

    /**
     * Returns all stored instances.
     *
     * @param instances array to return stored instances.
     * @return Error.
     */
    virtual Error GetAllInstances(Array<InstanceInfo>& instances) = 0;

    /**
     * Returns operation version.
     *
     * @return RetWithError<uint64_t>.
     */
    virtual RetWithError<uint64_t> GetOperationVersion() const = 0;

    /**
     * Sets operation version.
     *
     * @param version operation version.
     * @return Error.
     */
    virtual Error SetOperationVersion(uint64_t version) = 0;

    /**
     * Returns instances's override environment variables array.
     *
     * @param envVarsInstanceInfos[out] instances's override environment variables array.
     * @return Error.
     */
    virtual Error GetOverrideEnvVars(cloudprotocol::EnvVarsInstanceInfoArray& envVarsInstanceInfos) const = 0;

    /**
     * Sets instances's override environment variables array.
     *
     * @param envVarsInstanceInfos instances's override environment variables array.
     * @return Error.
     */
    virtual Error SetOverrideEnvVars(const cloudprotocol::EnvVarsInstanceInfoArray& envVarsInstanceInfos) = 0;

    /**
     * Returns online time.
     *
     * @return RetWithError<Time>.
     */
    virtual RetWithError<Time> GetOnlineTime() const = 0;

    /**
     * Sets online time.
     *
     * @param time online time.
     * @return Error.
     */
    virtual Error SetOnlineTime(const Time& time) = 0;

    /**
     * Destroys storage interface.
     */
    virtual ~StorageItf() = default;
};

/**
 * Launches service instances.
 */
class Launcher : public LauncherItf,
                 public runner::RunStatusReceiverItf,
                 private ConnectionSubscriberItf,
                 private NonCopyable {
public:
    /**
     * Creates launcher instance.
     */
    Launcher() = default;

    /**
     * Destroys launcher instance.
     */
    ~Launcher() = default;

    /**
     * Initializes launcher.
     *
     * @param statusReceiver status receiver instance.
     * @param runner runner instance.
     * @param storage storage instance.
     * @return Error.
     */
    Error Init(servicemanager::ServiceManagerItf& serviceManager, runner::RunnerItf& runner, OCISpecItf& ociManager,
        InstanceStatusReceiverItf& statusReceiver, StorageItf& storage, monitoring::ResourceMonitorItf& resourceMonitor,
        ConnectionPublisherItf& connectionPublisher);

    /**
     * Starts launcher.
     *
     * @return Error.
     */
    Error Start();

    /**
     * Stops launcher.
     *
     * @return Error.
     */
    Error Stop();

    /**
     * Runs specified instances.
     *
     * @param services services info array.
     * @param layers layers info array.
     * @param instances instances info array.
     * @param forceRestart forces restart already started instance.
     * @return Error.
     */
    Error RunInstances(const Array<ServiceInfo>& services, const Array<LayerInfo>& layers,
        const Array<InstanceInfo>& instances, bool forceRestart = false) override;

    /**
     * Overrides environment variables for specified instances.
     *
     * @param envVarsInfo environment variables info.
     * @param statuses[out] environment variables statuses.
     * @return Error
     */
    Error OverrideEnvVars(const Array<cloudprotocol::EnvVarsInstanceInfo>& envVarsInfo,
        cloudprotocol::EnvVarsInstanceStatusArray&                         statuses) override;

    /**
     * Sets cloud connection status.
     *
     * @param connected cloud connection status.
     * @return Error
     */
    Error SetCloudConnection(bool connected) override;

    /**
     * Updates run instances status.
     *
     * @param instances instances state.
     * @return Error.
     */
    Error UpdateRunStatus(const Array<runner::RunStatus>& instances) override;

    /**
     * Notifies publisher is connected.
     */
    void OnConnect() override;

    /**
     * Notifies publisher is disconnected.
     */
    void OnDisconnect() override;

    /**
     * Defines current operation version
     * if new functionality doesn't allow existing services to work properly,
     * this value should be increased.
     * It will force to remove all services and their storages before first start.
     */
    static constexpr uint32_t cOperationVersion = 9;

private:
    static constexpr auto cNumLaunchThreads = AOS_CONFIG_LAUNCHER_NUM_COOPERATE_LAUNCHES;
    static constexpr auto cThreadTaskSize   = AOS_CONFIG_LAUNCHER_THREAD_TASK_SIZE;
    static constexpr auto cThreadStackSize  = AOS_CONFIG_LAUNCHER_THREAD_STACK_SIZE;

    void  ProcessInstances(const Array<InstanceInfo>& instances, bool forceRestart = false);
    void  ProcessServices(const Array<ServiceInfo>& services);
    void  ProcessLayers(const Array<LayerInfo>& layers);
    void  SendRunStatus();
    void  StopInstances(const Array<InstanceInfo>& instances, bool forceRestart);
    void  StartInstances(const Array<InstanceInfo>& instances);
    void  CacheServices(const Array<InstanceInfo>& instances);
    void  UpdateInstanceServices();
    Error UpdateStorage(const Array<InstanceInfo>& instances);

    RetWithError<const Service&> GetService(const String& serviceID) const { return mCurrentServices.At(serviceID); }

    Error StartInstance(const InstanceInfo& info);
    Error StopInstance(const InstanceIdent& ident);
    Error RunLastInstances();

    ConnectionPublisherItf*            mConnectionPublisher {};
    servicemanager::ServiceManagerItf* mServiceManager {};
    runner::RunnerItf*                 mRunner {};
    InstanceStatusReceiverItf*         mStatusReceiver {};
    StorageItf*                        mStorage {};
    OCISpecItf*                        mOCIManager {};
    monitoring::ResourceMonitorItf*    mResourceMonitor {};

    StaticAllocator<sizeof(InstanceInfoStaticArray) * 2 + sizeof(ServiceInfoStaticArray) + sizeof(LayerInfoStaticArray)
        + sizeof(servicemanager::ServiceDataStaticArray) + sizeof(InstanceStatusStaticArray)>
        mAllocator;

    bool                                      mLaunchInProgress = false;
    Mutex                                     mMutex;
    Thread<cThreadTaskSize, cThreadStackSize> mThread;
    ThreadPool<cNumLaunchThreads, Max(cMaxNumInstances, cMaxNumServices, cMaxNumLayers), cThreadTaskSize,
        cThreadStackSize>
                        mLaunchPool;
    ConditionalVariable mCondVar;
    bool                mClose     = false;
    bool                mConnected = false;

    StaticMap<StaticString<cServiceIDLen>, Service, cMaxNumServices> mCurrentServices;
    StaticMap<InstanceIdent, Instance, cMaxNumInstances>             mCurrentInstances;
};

/** @}*/

} // namespace launcher
} // namespace sm
} // namespace aos

#endif
