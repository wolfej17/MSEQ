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

#include "MSEQFXParams.h"

#include <AK/Tools/Common/AkBankReadHelpers.h>

MSEQFXParams::MSEQFXParams()
{
}

MSEQFXParams::~MSEQFXParams()
{
}

MSEQFXParams::MSEQFXParams(const MSEQFXParams& in_rParams)
{
    RTPC = in_rParams.RTPC;
    NonRTPC = in_rParams.NonRTPC;
    m_paramChangeHandler.SetAllParamChanges();
}

AK::IAkPluginParam* MSEQFXParams::Clone(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, MSEQFXParams(*this));
}

AKRESULT MSEQFXParams::Init(AK::IAkPluginMemAlloc* in_pAllocator, const void* in_pParamsBlock, AkUInt32 in_ulBlockSize)
{
    if (in_ulBlockSize == 0)
    {
        // Initialize default SoundEngine parameters here. Ensure these align with initial Authoring params.
        RTPC.filter1Freq = 5000.0f;
        RTPC.filter1Gain = 0.0f;
        RTPC.filter1MidSide = true;
        RTPC.filter1Enable = true;
        RTPC.filter1Q = 1.0f;
        RTPC.filter1Type = 1; // High Shelf

        RTPC.filter2Freq = 1000.0f;
        RTPC.filter2Gain = 0.0f;
        RTPC.filter2MidSide = true;
        RTPC.filter2Enable = true;
        RTPC.filter2Q = 1.0f;

        RTPC.filter3Freq = 500.0f;
        RTPC.filter3Gain = 0.0f;
        RTPC.filter3MidSide = true;
        RTPC.filter3Enable = true;
        RTPC.filter3Q = 1.0f;

        RTPC.filter4Freq = 120.0f;
        RTPC.filter4Gain = 0.0f;
        RTPC.filter4MidSide = true;
        RTPC.filter4Enable = true;
        RTPC.filter4Q = 1.0f;
        RTPC.filter4Type = 3; // Low Shelf

        m_paramChangeHandler.SetAllParamChanges();
        return AK_Success;
    }

    return SetParamsBlock(in_pParamsBlock, in_ulBlockSize);
}

AKRESULT MSEQFXParams::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT MSEQFXParams::SetParamsBlock(const void* in_pParamsBlock, AkUInt32 in_ulBlockSize)
{
    AKRESULT eResult = AK_Success;
    AkUInt8* pParamsBlock = (AkUInt8*)in_pParamsBlock;

    // Read bank data here
    RTPC.filter1Freq = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter1Gain = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter1MidSide = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter1Enable = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter1Q = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter1Type = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize); // High Shelf

    RTPC.filter2Freq = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter2Gain = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter2MidSide = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter2Enable = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter2Q = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);

    RTPC.filter3Freq = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter3Gain = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter3MidSide = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter3Enable = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter3Q = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);

    RTPC.filter4Freq = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter4Gain = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter4MidSide = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter4Enable = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter4Q = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);
    RTPC.filter4Type = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);

    CHECKBANKDATASIZE(in_ulBlockSize, eResult);
    m_paramChangeHandler.SetAllParamChanges();

    return eResult;
}

AKRESULT MSEQFXParams::SetParam(AkPluginParamID in_paramID, const void* in_pValue, AkUInt32 in_ulParamSize)
{
    AKRESULT eResult = AK_Success;

    // Handle parameter change here
    switch (in_paramID)
    {
    case FILTER1ENABLE:
        RTPC.filter1Enable = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER1ENABLE);
        break;
    case FILTER1MIDSIDE:
        RTPC.filter1MidSide = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER1MIDSIDE);
        break;
    case FILTER1GAIN:
        RTPC.filter1Gain = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER1GAIN);
        break;
    case FILTER1FREQUENCY:
        RTPC.filter1Freq = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER1FREQUENCY);
        break;
    case FILTER1TYPE:
        RTPC.filter1Type = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER1TYPE);
        break;
    case FILTER1Q:
        RTPC.filter1Q = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER1Q);
        break;

    case FILTER2ENABLE:
        RTPC.filter2Enable = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER2ENABLE);
        break;
    case FILTER2MIDSIDE:
        RTPC.filter2MidSide = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER2MIDSIDE);
        break;
    case FILTER2GAIN:
        RTPC.filter2Gain = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER2GAIN);
        break;
    case FILTER2FREQUENCY:
        RTPC.filter2Freq = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER2FREQUENCY);
        break;
    case FILTER2Q:
        RTPC.filter2Q = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER2Q);
        break;

    case FILTER3ENABLE:
        RTPC.filter3Enable = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER3ENABLE);
        break;
    case FILTER3MIDSIDE:
        RTPC.filter3MidSide = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER3MIDSIDE);
        break;
    case FILTER3GAIN:
        RTPC.filter3Gain = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER3GAIN);
        break;
    case FILTER3FREQUENCY:
        RTPC.filter3Freq = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER3FREQUENCY);
        break;
    case FILTER3Q:
        RTPC.filter3Q = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER3Q);
        break;

    case FILTER4ENABLE:
        RTPC.filter4Enable = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER4ENABLE);
        break;
    case FILTER4MIDSIDE:
        RTPC.filter4MidSide = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER4MIDSIDE);
        break;
    case FILTER4GAIN:
        RTPC.filter4Gain = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER4GAIN);
        break;
    case FILTER4FREQUENCY:
        RTPC.filter4Freq = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER4FREQUENCY);
        break;
    case FILTER4TYPE:
        RTPC.filter4Type = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER4TYPE);
        break;
    case FILTER4Q:
        RTPC.filter4Q = *((AkReal32*)in_pValue);
        m_paramChangeHandler.SetParamChange(FILTER4Q);
        break;

    default:
        eResult = AK_InvalidParameter;
        break;
    }

    return eResult;
}
