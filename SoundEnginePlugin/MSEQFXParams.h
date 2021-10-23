/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

Apache License Usage

Alternatively, this file may be used under the Apache License, Version 2.0 (the
"Apache License"); you may not use this file except in compliance with the
Apache License. You may obtain a copy of the Apache License at
http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing, software distributed
under the Apache License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
OR CONDITIONS OF ANY KIND, either express or implied. See the Apache License for
the specific language governing permissions and limitations under the License.

  Copyright (c) 2021 Audiokinetic Inc.
*******************************************************************************/

#ifndef MSEQFXParams_H
#define MSEQFXParams_H

#include <AK/SoundEngine/Common/IAkPlugin.h>
#include <AK/Plugin/PluginServices/AkFXParameterChangeHandler.h>

// Add parameters IDs here, those IDs should map to the AudioEnginePropertyID
// attributes in the xml property definition.

static const AkPluginParamID FILTER1ENABLE = 0;
static const AkPluginParamID FILTER1MIDSIDE = 1;
static const AkPluginParamID FILTER1GAIN = 2;
static const AkPluginParamID FILTER1FREQUENCY = 3;
static const AkPluginParamID FILTER1TYPE = 4;
static const AkPluginParamID FILTER1Q = 5;

static const AkPluginParamID FILTER2ENABLE = 6;
static const AkPluginParamID FILTER2MIDSIDE = 7;
static const AkPluginParamID FILTER2GAIN = 8;
static const AkPluginParamID FILTER2FREQUENCY = 9;
static const AkPluginParamID FILTER2Q = 10;

static const AkPluginParamID FILTER3ENABLE = 11;
static const AkPluginParamID FILTER3MIDSIDE = 12;
static const AkPluginParamID FILTER3GAIN = 13;
static const AkPluginParamID FILTER3FREQUENCY = 14;
static const AkPluginParamID FILTER3Q = 15;

static const AkPluginParamID FILTER4ENABLE = 16;
static const AkPluginParamID FILTER4MIDSIDE = 17;
static const AkPluginParamID FILTER4GAIN = 18;
static const AkPluginParamID FILTER4FREQUENCY = 19;
static const AkPluginParamID FILTER4TYPE = 20;
static const AkPluginParamID FILTER4Q = 21;

static const AkUInt32 NUM_PARAMS = 22;

struct MSEQRTPCParams
{
    AkReal32 filter1Enable;
    AkReal32 filter1MidSide;
    AkReal32 filter1Gain;
    AkReal32 filter1Freq;
    AkReal32 filter1Type;
    AkReal32 filter1Q;

    AkReal32 filter2Enable;
    AkReal32 filter2MidSide;
    AkReal32 filter2Gain;
    AkReal32 filter2Freq;
    AkReal32 filter2Q;

    AkReal32 filter3Enable;
    AkReal32 filter3MidSide;
    AkReal32 filter3Gain;
    AkReal32 filter3Freq;
    AkReal32 filter3Q;

    AkReal32 filter4Enable;
    AkReal32 filter4MidSide;
    AkReal32 filter4Gain;
    AkReal32 filter4Freq;
    AkReal32 filter4Type;
    AkReal32 filter4Q;
};

struct MSEQNonRTPCParams
{
};

struct MSEQFXParams
    : public AK::IAkPluginParam
{
    MSEQFXParams();
    MSEQFXParams(const MSEQFXParams& in_rParams);

    ~MSEQFXParams();

    /// Create a duplicate of the parameter node instance in its current state.
    IAkPluginParam* Clone(AK::IAkPluginMemAlloc* in_pAllocator) override;

    /// Initialize the plug-in parameter node interface.
    /// Initializes the internal parameter structure to default values or with the provided parameter block if it is valid.
    AKRESULT Init(AK::IAkPluginMemAlloc* in_pAllocator, const void* in_pParamsBlock, AkUInt32 in_ulBlockSize) override;

    /// Called by the sound engine when a parameter node is terminated.
    AKRESULT Term(AK::IAkPluginMemAlloc* in_pAllocator) override;

    /// Set all plug-in parameters at once using a parameter block.
    AKRESULT SetParamsBlock(const void* in_pParamsBlock, AkUInt32 in_ulBlockSize) override;

    /// Update a single parameter at a time and perform the necessary actions on the parameter changes.
    AKRESULT SetParam(AkPluginParamID in_paramID, const void* in_pValue, AkUInt32 in_ulParamSize) override;

    AK::AkFXParameterChangeHandler<NUM_PARAMS> m_paramChangeHandler;

    MSEQRTPCParams RTPC;
    MSEQNonRTPCParams NonRTPC;
};

#endif // MSEQFXParams_H
