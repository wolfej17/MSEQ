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

#include "MSEQFX.h"
#include "../MSEQConfig.h"
#include "ExtendedDspFilters/Common.h"
#include "ExtendedDspFilters//Filter.h"

#include <AK/AkWwiseSDKVersion.h>

AK::IAkPlugin* CreateMSEQFX(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, MSEQFX());
}

AK::IAkPluginParam* CreateMSEQFXParams(AK::IAkPluginMemAlloc* in_pAllocator)
{
    return AK_PLUGIN_NEW(in_pAllocator, MSEQFXParams());
}

AK_IMPLEMENT_PLUGIN_FACTORY(MSEQFX, AkPluginTypeEffect, MSEQConfig::CompanyID, MSEQConfig::PluginID)

MSEQFX::MSEQFX()
    : m_pParams(nullptr)
    , m_pAllocator(nullptr)
    , m_pContext(nullptr)
{
}

MSEQFX::~MSEQFX()
{
}

AKRESULT MSEQFX::Init(AK::IAkPluginMemAlloc* in_pAllocator, AK::IAkEffectPluginContext* in_pContext, AK::IAkPluginParam* in_pParams, AkAudioFormat& in_rFormat)
{
    m_pParams = (MSEQFXParams*)in_pParams;
    m_pAllocator = in_pAllocator;
    m_pContext = in_pContext;

    sampleRate = in_rFormat.uSampleRate;

    peakOne = Parametric();
    peakTwo = Parametric();

    return AK_Success;
}

AKRESULT MSEQFX::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT MSEQFX::Reset()
{
    return AK_Success;
}

AKRESULT MSEQFX::GetPluginInfo(AkPluginInfo& out_rPluginInfo)
{
    out_rPluginInfo.eType = AkPluginTypeEffect;
    out_rPluginInfo.bIsInPlace = true;
    out_rPluginInfo.bCanProcessObjects = false;
    out_rPluginInfo.uBuildVersion = AK_WWISESDK_VERSION_COMBINED;
    return AK_Success;
}

void MSEQFX::Execute(AkAudioBuffer* io_pBuffer)
{
    const int uNumChannels = io_pBuffer->NumChannels();
    const int uBufferLength = io_pBuffer->uValidFrames;

    // Calculate Coefficients
    highShelf.setup(filterOrder, sampleRate, m_pParams->RTPC.filter1Freq, m_pParams->RTPC.filter1Gain);
    lowShelf.setup(filterOrder, sampleRate, m_pParams->RTPC.filter4Freq, m_pParams->RTPC.filter4Gain);
    peakOne.calcCoeffs(m_pParams->RTPC.filter2Gain, m_pParams->RTPC.filter2Freq, m_pParams->RTPC.filter2QualityFactor, sampleRate);
    peakTwo.calcCoeffs(m_pParams->RTPC.filter3Gain, m_pParams->RTPC.filter3Freq, m_pParams->RTPC.filter3QualityFactor, sampleRate);

    float* channels[1];

    constructMidSideBuffers(io_pBuffer, uBufferLength);

    // High Shelf
    if (m_pParams->RTPC.filter1MidSide == true) { channels[0] = sideBuffer; } else { channels[0] = midBuffer; }
    if (m_pParams->RTPC.filter1Gain != 0) { highShelf.process(uBufferLength, channels); }

    // Band 2
    if (m_pParams->RTPC.filter2MidSide == true) { channels[0] = sideBuffer; } else { channels[0] = midBuffer; }
    if (m_pParams->RTPC.filter2Gain != 0) { peakOne.processBlock(channels[0], uBufferLength, false); }

    // Band 3
    if (m_pParams->RTPC.filter3MidSide == true) { channels[0] = sideBuffer; } else { channels[0] = midBuffer; }
    if (m_pParams->RTPC.filter3Gain != 0) { peakTwo.processBlock(channels[0], uBufferLength, false); }

    // Low Shelf
    if (m_pParams->RTPC.filter4MidSide == true) { channels[0] = sideBuffer; } else { channels[0] = midBuffer; }
    if (m_pParams->RTPC.filter4Gain != 0) { lowShelf.process(uBufferLength, channels); }

    deconstructMidSideBuffers(io_pBuffer, uBufferLength);
}

void MSEQFX::constructMidSideBuffers(AkAudioBuffer* inBuf, int bufferLength)
{
    float* channel1 = inBuf->GetChannel(0);
    float* channel2 = inBuf->GetChannel(1);

    sideBuffer = new float[bufferLength];
    midBuffer = new float[bufferLength];

    for (int i = 0; i < bufferLength; ++i)
    {
        float midSample = (channel1[i] + channel2[i]) / float(sqrt(2));  // Cast to avoid arithmetic overflow
        float sideSample = (channel1[i] - channel2[i]) / float(sqrt(2)); // Cast to avoid arithmetic overflow

        midBuffer[i] = midSample;
        sideBuffer[i] = sideSample;
    }
}

void MSEQFX::deconstructMidSideBuffers(AkAudioBuffer* inBuf, int bufferLength)
{
    float* channel1 = inBuf->GetChannel(0);
    float* channel2 = inBuf->GetChannel(1);

    for (int i = 0; i < bufferLength; ++i)
    {
        float chanOneSample = (midBuffer[i] + sideBuffer[i]) / float(sqrt(2)); // Cast to avoid arithmetic overflow
        float chanTwoSample = (midBuffer[i] - sideBuffer[i]) / float(sqrt(2)); // Cast to avoid arithmetic overflow

        channel1[i] = chanOneSample;
        channel2[i] = chanTwoSample;
    }

    delete[] midBuffer;
    delete[] sideBuffer;
}

double MSEQFX::computeUpperCutoffFrequency(double q, double f0)
{
    return f0 * (sqrt(1 + (1 / (4 * pow(q, 2)))) + (1 / (2 * q)));
}

double MSEQFX::computeLowerCutoffFrequency(double q, double f0)
{
    return f0 * (sqrt(1 + (1 / (4 * pow(q, 2)))) - (1 / (2 * q)));
}

AKRESULT MSEQFX::TimeSkip(AkUInt32 in_uFrames)
{
    return AK_DataReady;
}