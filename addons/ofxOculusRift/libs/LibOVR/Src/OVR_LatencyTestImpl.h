/************************************************************************************

Filename    :   OVR_LatencyTestImpl.h
Content     :   Latency Tester specific implementation.
Created     :   March 7, 2013
Authors     :   Lee Cooper

Copyright   :   Copyright 2013 Oculus VR, Inc. All Rights reserved.

Use of this software is subject to the terms of the Oculus license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.

*************************************************************************************/

#ifndef OVR_LatencyTestImpl_h
#define OVR_LatencyTestImpl_h

#include "OVR_HIDDeviceImpl.h"

namespace OVR {

struct LatencyTestSamplesMessage;
struct LatencyTestButtonMessage;
struct LatencyTestStartedMessage;
struct LatencyTestColorDetectedMessage;

//-------------------------------------------------------------------------------------
// LatencyTestDeviceFactory enumerates Oculus Latency Tester devices.
class LatencyTestDeviceFactory : public DeviceFactory
{
public:
    static LatencyTestDeviceFactory Instance;

    // Enumerates devices, creating and destroying relevant objects in manager.
    virtual void EnumerateDevices(EnumerateVisitor& visitor);

protected:
    DeviceManager* getManager() const { return (DeviceManager*) pManager; }   
};


// Describes a single a Oculus Latency Tester device and supports creating its instance.
class LatencyTestDeviceCreateDesc : public HIDDeviceCreateDesc
{
public:
    LatencyTestDeviceCreateDesc(DeviceFactory* factory, const HIDDeviceDesc& hidDesc)
        : HIDDeviceCreateDesc(factory, Device_LatencyTester, hidDesc) { }
    
    virtual DeviceCreateDesc* Clone() const
    {
        return new LatencyTestDeviceCreateDesc(*this);
    }

    virtual DeviceBase* NewDeviceInstance();

    virtual MatchResult MatchDevice(const DeviceCreateDesc& other,
                                    DeviceCreateDesc**) const
    {
        if ((other.Type == Device_LatencyTester) && (pFactory == other.pFactory))
        {            
            const LatencyTestDeviceCreateDesc& s2 = (const LatencyTestDeviceCreateDesc&) other;
            if ((HIDDesc.Path == s2.HIDDesc.Path) &&
                (HIDDesc.SerialNumber == s2.HIDDesc.SerialNumber))
                return Match_Found;
        }
        return Match_None;
    }

    virtual bool        GetDeviceInfo(DeviceInfo* info) const;
};


//-------------------------------------------------------------------------------------
// ***** OVR::LatencyTestDeviceImpl

// Oculus Latency Tester interface.

class LatencyTestDeviceImpl : public HIDDeviceImpl<OVR::LatencyTestDevice>
{
public:
     LatencyTestDeviceImpl(LatencyTestDeviceCreateDesc* createDesc);
    ~LatencyTestDeviceImpl();

    // DeviceCommon interface.
    virtual bool Initialize(DeviceBase* parent);
    virtual void Shutdown();

    // DeviceManagerThread::Notifier interface.
    virtual void OnInputReport(UByte* pData, UInt32 length);

    // LatencyTesterDevice interface
    virtual bool SetConfiguration(const OVR::LatencyTestConfiguration& configuration, bool waitFlag = false);
    virtual bool GetConfiguration(OVR::LatencyTestConfiguration* configuration);

    virtual bool SetCalibrate(const OVR::LatencyTestCalibrate& calibrate, bool waitFlag = false);
    virtual bool GetCalibrate(OVR::LatencyTestCalibrate* calibrate);

    virtual bool SetStartTest(const OVR::LatencyTestStartTest& start, bool waitFlag = false);
    virtual bool SetDisplay(const LatencyTestDisplay& display, bool waitFlag = false);

protected:
    bool    openDevice(const char** errorFormatString);
    void    closeDevice();
    void    closeDeviceOnIOError();

    bool    initializeRead();
    bool    processReadResult();

    bool    setConfiguration(const OVR::LatencyTestConfiguration& configuration);
    bool    getConfiguration(OVR::LatencyTestConfiguration* configuration);
    bool    setCalibrate(const OVR::LatencyTestCalibrate& calibrate);
    bool    getCalibrate(OVR::LatencyTestCalibrate* calibrate);
    bool    setStartTest(const OVR::LatencyTestStartTest& start);
    bool    setDisplay(const OVR::LatencyTestDisplay& display);

    // Called for decoded messages
    void onLatencyTestSamplesMessage(LatencyTestSamplesMessage* message);
    void onLatencyTestButtonMessage(LatencyTestButtonMessage* message);
    void onLatencyTestStartedMessage(LatencyTestStartedMessage* message);
    void onLatencyTestColorDetectedMessage(LatencyTestColorDetectedMessage* message);

};

} // namespace OVR

#endif // OVR_LatencyTestImpl_h
