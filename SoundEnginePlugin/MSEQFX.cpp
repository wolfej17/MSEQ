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
#include "DspFilters/Common.h"
#include "DspFilters/Filter.h"

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
    for (int i = 0; i < 2; i++)
    {
        outerBands[i] = nullptr;
        outerBandsParams[i] = nullptr;
        m_Filters[i] = nullptr;
        innerBands[i] = nullptr;
    }
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


    for (int i = 0; i < 4; i++)
    {
        if (i < 2)
        {
            if (i % 2 == 0)
            {
                switchFilterType(&outerBands[i], &outerBandsParams[i], &m_Filters[i], &bandOneType, &m_pParams->RTPC.filter1Type);
                setFilterParameters(outerBands[i], m_Filters[i], sampleRate, m_pParams->RTPC.filter1Freq, m_pParams->RTPC.filter1Gain, m_pParams->RTPC.filter1Q);
            }
            else
            {
                switchFilterType(&outerBands[i], &outerBandsParams[i], &m_Filters[i], &bandFourType, &m_pParams->RTPC.filter4Type);
                setFilterParameters(outerBands[i], m_Filters[i], sampleRate, m_pParams->RTPC.filter4Freq, m_pParams->RTPC.filter4Gain, m_pParams->RTPC.filter4Q);
            }
        }
        else 
        {
            innerBands[i - 2] = new Parametric();
        }
    }

    return AK_Success;
}

AKRESULT MSEQFX::Term(AK::IAkPluginMemAlloc* in_pAllocator)
{
    
    AK_PLUGIN_DELETE(in_pAllocator, this);
    return AK_Success;
}

AKRESULT MSEQFX::Reset()
{
    for (int i = 0; i < 2; i++)
    {
        resetCheck(outerBands[i]);
        resetCheck(outerBandsParams[i]);
        resetCheck(m_Filters[i]);
        innerBands[i]->reset();
    }
    return AK_Success;
}

void MSEQFX::resetCheck(Dsp::Filter* filterBand)
{
    if (filterBand != nullptr)
    {
        filterBand->reset();
    }
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

    float* channels[1];
    constructMidSideBuffers(io_pBuffer, uBufferLength);

    for (int i = 0; i < 4; i++)
    {
        if (i < 2)
        {
            if (outerBands[i] != nullptr)
            {
                /// TODO: Handle parameter smoothing.
                /// TODO: Each Dsp::Filter can be modified to hold Authoring parameter data.
                /// TODO: Use AK Type Alias' and document.

                if (i % 2 == 0)
                {
                    if (m_pParams->RTPC.filter1Enable)
                    {
                        if (m_pParams->RTPC.filter1MidSide == true) { channels[0] = sideBuffer; }
                        else { channels[0] = midBuffer; }

                        switchFilterType(&outerBands[i], &outerBandsParams[i], &m_Filters[i], &bandOneType, &m_pParams->RTPC.filter1Type);
                        setFilterParameters(outerBands[i], m_Filters[i], sampleRate, m_pParams->RTPC.filter1Freq, m_pParams->RTPC.filter1Gain, m_pParams->RTPC.filter1Q);
                        outerBands[i]->process(uBufferLength, channels);
                    }

                }
                else
                {
                    if (m_pParams->RTPC.filter4Enable)
                    {
                        if (m_pParams->RTPC.filter4MidSide == true) { channels[0] = sideBuffer; }
                        else { channels[0] = midBuffer; }
                        
                        switchFilterType(&outerBands[i], &outerBandsParams[i], &m_Filters[i], &bandFourType, &m_pParams->RTPC.filter4Type);
                        setFilterParameters(outerBands[i], m_Filters[i], sampleRate, m_pParams->RTPC.filter4Freq, m_pParams->RTPC.filter4Gain, m_pParams->RTPC.filter4Q);
                        outerBands[i]->process(uBufferLength, channels);
                        
                    }
                }
            }
        }
        else
        {
            if (innerBands[i - 2] != nullptr)
            {
                if (i % 2 == 0)
                {
                    if (m_pParams->RTPC.filter2Enable)
                    {
                        if (m_pParams->RTPC.filter2MidSide == true) { channels[0] = sideBuffer; }
                        else { channels[0] = midBuffer; }
                        innerBands[i - 2]->calcCoeffs(m_pParams->RTPC.filter2Gain, m_pParams->RTPC.filter2Freq, m_pParams->RTPC.filter2Q, sampleRate);
                        innerBands[i - 2]->processBlock(channels[0], uBufferLength, false);
                    }
                }
                else
                {
                    if (m_pParams->RTPC.filter3Enable)
                    {
                        if (m_pParams->RTPC.filter3MidSide == true) { channels[0] = sideBuffer; }
                        else { channels[0] = midBuffer; }
                        innerBands[i - 2]->calcCoeffs(m_pParams->RTPC.filter3Gain, m_pParams->RTPC.filter3Freq, m_pParams->RTPC.filter3Q, sampleRate);
                        innerBands[i - 2]->processBlock(channels[0], uBufferLength, false);
                    }
                }
            }
        }
    }
    deconstructMidSideBuffers(io_pBuffer, uBufferLength);
}

template <class DesignType, class StateType>
void MSEQFX::createFilterDesign(Dsp::Filter** pFilter, Dsp::Filter** pAudioFilter)
{
    *pAudioFilter = new Dsp::SmoothedFilterDesign <DesignType, 1, StateType>(1024);
}

template <class DesignType>
void MSEQFX::createFilterState(Dsp::Filter** pFilter, Dsp::Filter** pAudioFilter)
{
    *pFilter = new Dsp::FilterDesign <DesignType, 1>;
    createFilterDesign <DesignType, Dsp::DirectFormII>(pFilter, pAudioFilter);
}

void MSEQFX::setFilterParameters(Dsp::Filter* filterBand, Dsp::Filter* filterBandParams, float sampRate, float freq, float gain, float q)
{
    if (filterBandParams != nullptr)
    {
        filterBandParams->setParamById(Dsp::idSampleRate, sampleRate);
        filterBandParams->setParamById(Dsp::idFrequency, freq);
        filterBandParams->setParamById(Dsp::idGain, gain);
        filterBandParams->setParamById(Dsp::idQ, q);
        filterBandParams->setParamById(Dsp::idOrder, filterOrder);

        filterBand->copyParamsFrom(filterBandParams);
    }
}

void MSEQFX::switchFilterType(Dsp::Filter** filterBand, Dsp::Filter** filterBandParams, Dsp::Filter** m_Filter, AkReal32 *soundEngineBandType, AkReal32 *authoringBandType)
{
    if (*soundEngineBandType != *authoringBandType)
    {

        int authoringBand = *authoringBandType;
        switch (authoringBand)
        {
        case HIGHSHELF:
            createFilterState <Dsp::Butterworth::Design::HighShelf <filterOrder> >(filterBand, filterBandParams);
            break;
        case LOWPASS:
            createFilterState <Dsp::Butterworth::Design::LowPass <filterOrder> >(filterBand, filterBandParams);
            break;
        case HIGHPASS:
            createFilterState <Dsp::Butterworth::Design::HighPass <filterOrder> >(filterBand, filterBandParams);
            break;
        case LOWSHELF:
            createFilterState <Dsp::Butterworth::Design::LowShelf <filterOrder> >(filterBand, filterBandParams);
            break;
        default:
            createFilterState <Dsp::Butterworth::Design::HighShelf <filterOrder> >(filterBand, filterBandParams);
            break;
        }

        // Reset between changing filter types?
        Dsp::Filter* mFilt = *m_Filter;
        Dsp::Filter* filtParams = *filterBandParams;
        Dsp::Filter* audioFilt = *filterBand;

        if (mFilt != nullptr)
        {
            filtParams->copyParamsFrom(mFilt);
        }
        *m_Filter = *filterBandParams;
        mFilt = *m_Filter;
        audioFilt ->setParams(mFilt->getParams());

        *soundEngineBandType = *authoringBandType;
    }
}

void MSEQFX::constructMidSideBuffers(AkAudioBuffer* inBuf, int bufferLength)
{
    float* channel1 = inBuf->GetChannel(0);
    float* channel2 = inBuf->GetChannel(1);

    sideBuffer = new float[bufferLength];
    midBuffer = new float[bufferLength];

    for (int i = 0; i < bufferLength; ++i)
    {
        float midSample = (channel1[i] + channel2[i]) / float(sqrt(2));
        float sideSample = (channel1[i] - channel2[i]) / float(sqrt(2));

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
        float chanOneSample = (midBuffer[i] + sideBuffer[i]) / float(sqrt(2));
        float chanTwoSample = (midBuffer[i] - sideBuffer[i]) / float(sqrt(2));

        channel1[i] = chanOneSample;
        channel2[i] = chanTwoSample;
    }

    if (midBuffer != nullptr)
    { 
        delete midBuffer;
        midBuffer = nullptr;
    }
    if (sideBuffer != nullptr)
    {
        delete sideBuffer;
        sideBuffer = nullptr;
    }
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