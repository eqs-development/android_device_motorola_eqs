/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <android/hardware/sensors/2.1/types.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include <condition_variable>
#include <fstream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <linux/input.h>

using ::android::hardware::sensors::V1_0::OperationMode;
using ::android::hardware::sensors::V1_0::Result;
using ::android::hardware::sensors::V2_1::Event;
using ::android::hardware::sensors::V2_1::SensorInfo;
using ::android::hardware::sensors::V2_1::SensorType;

namespace android {
namespace hardware {
namespace sensors {
namespace V2_1 {
namespace subhal {
namespace implementation {

class ISensorsEventCallback {
  public:
    virtual ~ISensorsEventCallback(){};
    virtual void postEvents(const std::vector<Event>& events, bool wakeup) = 0;
};

class Sensor {
  public:
    Sensor(int32_t sensorHandle, ISensorsEventCallback* callback);
    virtual ~Sensor();

    const SensorInfo& getSensorInfo() const;
    virtual void batch(int32_t samplingPeriodNs);
    virtual void activate(bool enable);
    virtual Result flush();

    virtual void setOperationMode(OperationMode mode);
    bool supportsDataInjection() const;
    Result injectEvent(const Event& event);

  protected:
    virtual void run();
    virtual std::vector<Event> readEvents();
    static void startThread(Sensor* sensor);

    bool isWakeUpSensor();

    bool mIsEnabled;
    int64_t mSamplingPeriodNs;
    int64_t mLastSampleTimeNs;
    SensorInfo mSensorInfo;

    std::atomic_bool mStopThread;
    std::condition_variable mWaitCV;
    std::mutex mRunMutex;
    std::thread mRunThread;

    ISensorsEventCallback* mCallback;

    OperationMode mMode;
};

class OneShotSensor : public Sensor {
  public:
    OneShotSensor(int32_t sensorHandle, ISensorsEventCallback* callback);

    virtual void batch(int32_t /* samplingPeriodNs */) override {}

    virtual Result flush() override { return Result::BAD_VALUE; }
};

class InputEventOneShotSensor : public OneShotSensor {
  public:
    InputEventOneShotSensor(int32_t sensorHandle, ISensorsEventCallback* callback,
                            const std::string& pollPath, const std::string& enablePath,
                            int enableValue, short keyValue, const std::string& name,
                            const std::string& typeAsString, SensorType type);
    virtual ~InputEventOneShotSensor() override;

    virtual void activate(bool enable) override;
    virtual void setOperationMode(OperationMode mode) override;
    virtual std::vector<Event> readEvents() override;
    virtual void fillEventData(Event& event);

  protected:
    virtual void run() override;

    std::ofstream mGestureEnable;

  private:
    void interruptPoll();

    struct pollfd mPolls[2];
    int mWaitPipeFd[2];
    int mPollFds[1];
    short mKeyValue;
};

const std::string kDoubleTapInputEventPath = "/dev/input/event10";
const std::string kGestureEnablePath = "/sys/class/touchscreen/primary/gesture";

const int kEnableDt2w = 49;
const int kEnableFodPressed = 17;

class InputEventDT2WSensor : public InputEventOneShotSensor {
  public:
    InputEventDT2WSensor(int32_t sensorHandle, ISensorsEventCallback* callback)
        : InputEventOneShotSensor(
            sensorHandle, callback, kDoubleTapInputEventPath,
            kGestureEnablePath, kEnableDt2w, KEY_F4, "DT2W sensor",
            "org.lineageos.sensor.dt2w", static_cast<SensorType>(static_cast<int32_t>(SensorType::DEVICE_PRIVATE_BASE) + 1)) {}
};

class InputEventUdfpsSensor : public InputEventOneShotSensor {
  public:
    InputEventUdfpsSensor(int32_t sensorHandle, ISensorsEventCallback* callback)
        : InputEventOneShotSensor(
            sensorHandle, callback, kDoubleTapInputEventPath,
            kGestureEnablePath, kEnableFodPressed, KEY_F2, "UDFPS sensor",
            "org.lineageos.sensor.udfps", static_cast<SensorType>(static_cast<int32_t>(SensorType::DEVICE_PRIVATE_BASE) + 2)) {}
};

}  // namespace implementation
}  // namespace subhal
}  // namespace V2_1
}  // namespace sensors
}  // namespace hardware
}  // namespace android
