#include <RendererCore/PCH.h>
#include <RendererCore/Pipeline/Passes/ScreenSpaceAmbientOcclusionPass.h>
#include <RendererCore/Pipeline/View.h>
#include <RendererCore/RenderContext/RenderContext.h>
#include <RendererCore/GPUResourcePool/GPUResourcePool.h>
#include <RendererFoundation/Context/Profiling.h>

#include <CoreUtils/Geometry/GeomUtils.h>


EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezScreenSpaceAmbientOcclusionPass, 1, ezRTTIDefaultAllocator<ezScreenSpaceAmbientOcclusionPass>)
{
  EZ_BEGIN_PROPERTIES
  {
    EZ_MEMBER_PROPERTY("Depth", m_PinDepthInput),
    EZ_MEMBER_PROPERTY("AmbientObscurance", m_PinOutput),
    EZ_ACCESSOR_PROPERTY("LineToLineDistance", GetLineToLinePixelOffset, SetLineToLinePixelOffset)->AddAttributes(new ezDefaultValueAttribute(2), new ezClampValueAttribute(1, 128)),
    EZ_ACCESSOR_PROPERTY("LineSampleDistanceFactor", GetLineSamplePixelOffset, SetLineSamplePixelOffset)->AddAttributes(new ezDefaultValueAttribute(2), new ezClampValueAttribute(1, 128)),
    EZ_ACCESSOR_PROPERTY("OcclusionFalloff", GetOcclusionFalloff, SetOcclusionFalloff)->AddAttributes(new ezDefaultValueAttribute(0.25f), new ezClampValueAttribute(0.0f, 2.0f)),
    EZ_ACCESSOR_PROPERTY("DepthCutoffFactor", GetDepthCutoffFactor, SetDepthCutoffFactor)->AddAttributes(new ezDefaultValueAttribute(8.0f), new ezClampValueAttribute(0.0f, 15.0f)),
  }
  EZ_END_PROPERTIES
}
EZ_END_DYNAMIC_REFLECTED_TYPE

namespace
{
  float HaltonSequence(int base, int j)
  {
    static int primes[61] = { 2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,
      83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,
      191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283 };

    EZ_ASSERT_DEV(base < 61, "Don't have prime number for this base.");

    // Halton sequence with reverse permutation
    const int p = primes[base];
    float h = 0.0f;
    float f = 1.0f / static_cast<float>(p);
    float fct = f;
    while (j > 0)
    {
      int i = j % p;
      h += (i == 0 ? i : p - i) * fct;
      j /= p;
      fct *= f;
    }
    return h;
  }
}

ezScreenSpaceAmbientOcclusionPass::ezScreenSpaceAmbientOcclusionPass()
  : ezRenderPipelinePass("ScreenSpaceAmbientOcclusionPass")
  , m_uiLineToLinePixelOffset(2)
  , m_uiLineSamplePixelOffsetFactor(2)
  , m_bSweepDataDirty(true)
{
  {
    // Load shader.
    m_hShaderLineSweep = ezResourceManager::LoadResource<ezShaderResource>("Shaders/Pipeline/SSAOSweep.ezShader");
    EZ_ASSERT_DEV(m_hShaderLineSweep.IsValid(), "Could not lsao sweep shader!");
    m_hShaderGather = ezResourceManager::LoadResource<ezShaderResource>("Shaders/Pipeline/SSAOGather.ezShader");
    EZ_ASSERT_DEV(m_hShaderGather.IsValid(), "Could not lsao gather shader!");
  }

  {
    m_hLineSweepCB = ezRenderContext::CreateConstantBufferStorage<ezSSAOConstants>();
    ezSSAOConstants* cb = ezRenderContext::GetConstantBufferData<ezSSAOConstants>(m_hLineSweepCB);
    cb->DepthCutoffFactor = 8.0f;
    cb->OcclusionFalloff = 0.25f;
  }
}

ezScreenSpaceAmbientOcclusionPass::~ezScreenSpaceAmbientOcclusionPass()
{
  DestroyLineSweepData();

  ezRenderContext::DeleteConstantBufferStorage(m_hLineSweepCB);
  m_hLineSweepCB.Invalidate();
}

bool ezScreenSpaceAmbientOcclusionPass::GetRenderTargetDescriptions(const ezView& view, const ezArrayPtr<ezGALTextureCreationDescription*const> inputs, ezArrayPtr<ezGALTextureCreationDescription> outputs)
{
  EZ_ASSERT_DEBUG(inputs.GetCount() == 1, "Unexpected number of inputs for ezScreenSpaceAmbientOcclusionPass.");

  // Depth
  if (!inputs[m_PinDepthInput.m_uiInputIndex])
  {
    ezLog::Error("No depth input connected to ssao pass!");
    return false;
  }
  if (!inputs[m_PinDepthInput.m_uiInputIndex]->m_bAllowShaderResourceView)
  {
    ezLog::Error("All ssao pass inputs must allow shader resoure view.");
    return false;
  }

  // Output format maches input format but is f16.
  outputs[m_PinOutput.m_uiOutputIndex] = *inputs[m_PinDepthInput.m_uiInputIndex];
  outputs[m_PinOutput.m_uiOutputIndex].m_Format = ezGALResourceFormat::RUShortNormalized; // 16 bit integer 0-1

  return true;
}

void ezScreenSpaceAmbientOcclusionPass::InitRenderPipelinePass(const ezArrayPtr<ezRenderPipelinePassConnection* const> inputs, const ezArrayPtr<ezRenderPipelinePassConnection* const> outputs)
{
  // Todo: Support half resolution.
  SetupLineSweepData(ezVec2I32(inputs[m_PinDepthInput.m_uiInputIndex]->m_Desc.m_uiWidth, inputs[m_PinDepthInput.m_uiInputIndex]->m_Desc.m_uiHeight));
}

void ezScreenSpaceAmbientOcclusionPass::Execute(const ezRenderViewContext& renderViewContext, const ezArrayPtr<ezRenderPipelinePassConnection* const> inputs, const ezArrayPtr<ezRenderPipelinePassConnection* const> outputs)
{
  if(m_bSweepDataDirty)
    SetupLineSweepData(ezVec2I32(inputs[m_PinDepthInput.m_uiInputIndex]->m_Desc.m_uiWidth, inputs[m_PinDepthInput.m_uiInputIndex]->m_Desc.m_uiHeight));

  if (outputs[m_PinOutput.m_uiOutputIndex])
  {
    ezGALDevice* pDevice = ezGALDevice::GetDefaultDevice();
    ezGALContext* pGALContext = renderViewContext.m_pRenderContext->GetGALContext();

    // Set rendertarget immediately to ensure that depth buffer is no longer bound (we need it right away in the sweeping part)
    ezGALResourceViewCreationDescription rvcd;
    rvcd.m_hTexture = inputs[m_PinDepthInput.m_uiInputIndex]->m_TextureHandle;
    ezGALResourceViewHandle hDepthInputView = ezGALDevice::GetDefaultDevice()->CreateResourceView(rvcd);
    ezGALRenderTagetSetup renderTargetSetup;
    renderTargetSetup.SetRenderTarget(0, pDevice->GetDefaultRenderTargetView(outputs[m_PinOutput.m_uiOutputIndex]->m_TextureHandle));
    renderViewContext.m_pRenderContext->SetViewportAndRenderTargetSetup(renderViewContext.m_pViewData->m_ViewPortRect, renderTargetSetup);


    // Constant buffer for both passes.
    renderViewContext.m_pRenderContext->BindConstantBuffer("ezSSAOConstants", m_hLineSweepCB);
    // Depth buffer for both passes.
    renderViewContext.m_pRenderContext->BindTexture2D(ezGALShaderStage::PixelShader, "DepthBuffer", hDepthInputView);
    renderViewContext.m_pRenderContext->BindTexture2D(ezGALShaderStage::ComputeShader, "DepthBuffer", hDepthInputView);


    // Line Sweep part (compute)
    {
      EZ_PROFILE_AND_MARKER(pGALContext, "Line Sweep");

      renderViewContext.m_pRenderContext->BindShader(m_hShaderLineSweep);
      renderViewContext.m_pRenderContext->BindBuffer(ezGALShaderStage::ComputeShader, "LineInstructions", m_hLineSweepInfoSRV);
      renderViewContext.m_pRenderContext->BindUAV("LineSweepOutputBuffer", m_hLineSweepOutputUAV);
      renderViewContext.m_pRenderContext->Dispatch(m_numSweepLines / SSAO_LINESWEEP_THREAD_GROUP + (m_numSweepLines % SSAO_LINESWEEP_THREAD_GROUP != 0 ? 1 : 0));
    }

    {
      EZ_PROFILE_AND_MARKER(pGALContext, "Gather");

      // Manually unbind UAV. TODO, this should be done automatically.
      pGALContext->SetUnorderedAccessView(0, ezGALUnorderedAccessViewHandle());

      renderViewContext.m_pRenderContext->BindShader(m_hShaderGather);

      renderViewContext.m_pRenderContext->BindBuffer(ezGALShaderStage::PixelShader, "LineInstructions", m_hLineSweepInfoSRV);
      renderViewContext.m_pRenderContext->BindBuffer(ezGALShaderStage::PixelShader, "LineSweepOutputBuffer", m_hLineSweepOutputSRV);

      renderViewContext.m_pRenderContext->BindMeshBuffer(ezGALBufferHandle(), ezGALBufferHandle(), nullptr, ezGALPrimitiveTopology::Triangles, 1);
      renderViewContext.m_pRenderContext->DrawMeshBuffer();
    }
  }
}


void ezScreenSpaceAmbientOcclusionPass::SetLineToLinePixelOffset(ezUInt32 uiPixelOffset)
{
  m_uiLineToLinePixelOffset = uiPixelOffset;
  m_bSweepDataDirty = true;
}

void ezScreenSpaceAmbientOcclusionPass::SetLineSamplePixelOffset(ezUInt32 uiPixelOffset)
{
  m_uiLineSamplePixelOffsetFactor = uiPixelOffset;
  m_bSweepDataDirty = true;
}

float ezScreenSpaceAmbientOcclusionPass::GetDepthCutoffFactor() const
{
  ezSSAOConstants* cb = ezRenderContext::GetConstantBufferData<ezSSAOConstants>(m_hLineSweepCB);
  return cb->DepthCutoffFactor;
}

void ezScreenSpaceAmbientOcclusionPass::SetDepthCutoffFactor(float fDepthCutoffFactor)
{
  ezSSAOConstants* cb = ezRenderContext::GetConstantBufferData<ezSSAOConstants>(m_hLineSweepCB);
  cb->DepthCutoffFactor = fDepthCutoffFactor;
}

float ezScreenSpaceAmbientOcclusionPass::GetOcclusionFalloff() const
{
  ezSSAOConstants* cb = ezRenderContext::GetConstantBufferData<ezSSAOConstants>(m_hLineSweepCB);
  return cb->OcclusionFalloff;
}

void ezScreenSpaceAmbientOcclusionPass::SetOcclusionFalloff(float fFalloff)
{
  ezSSAOConstants* cb = ezRenderContext::GetConstantBufferData<ezSSAOConstants>(m_hLineSweepCB);
  cb->OcclusionFalloff = fFalloff;
}

void ezScreenSpaceAmbientOcclusionPass::DestroyLineSweepData()
{
  ezGALDevice* device = ezGALDevice::GetDefaultDevice();

  if (!m_hLineSweepOutputUAV.IsInvalidated())
    device->DestroyUnorderedAccessView(m_hLineSweepOutputUAV);
  m_hLineSweepOutputUAV.Invalidate();

  if (!m_hLineSweepOutputSRV.IsInvalidated())
    device->DestroyResourceView(m_hLineSweepOutputSRV);
  m_hLineSweepOutputSRV.Invalidate();

  if (!m_hLineSweepOutputBuffer.IsInvalidated())
    device->DestroyBuffer(m_hLineSweepOutputBuffer);
  m_hLineSweepOutputBuffer.Invalidate();

  if (!m_hLineInfoBuffer.IsInvalidated())
    device->DestroyBuffer(m_hLineInfoBuffer);
  m_hLineInfoBuffer.Invalidate();
}

void ezScreenSpaceAmbientOcclusionPass::SetupLineSweepData(const ezVec2I32& imageResolution)
{
  DestroyLineSweepData();

  ezDynamicArray<LineInstruction> lineInstructions;
  ezUInt32 totalNumberOfSamples = 0;
  ezSSAOConstants* cb = ezRenderContext::GetConstantBufferData<ezSSAOConstants>(m_hLineSweepCB);
  cb->LineToLinePixelOffset = m_uiLineToLinePixelOffset;

  // Compute general information per direction and create line instructions.

  // As long as we don't span out different line samplings accross multiple frames, the number of prepared directions here is always equal to the number of directions per frame.
  // Note that if we were to do temporal sampling with a different line set every frame, we would need to precompute all *possible* sampling directions still as a whole here!
  ezVec2I32 samplingDir[NUM_SWEEP_DIRECTIONS_PER_FRAME];
  {
    constexpr int numSweepDirs = NUM_SWEEP_DIRECTIONS_PER_FRAME;

    // As described in the paper, all directions are aligned so that we always hit  pixels on a square.
    static_assert(numSweepDirs % 4 == 0, "Invalid number of sweep directions for LSAO!");
   // static_assert((numSweepDirs * NUM_SWEEP_DIRECTIONS_PER_PIXEL) % 9 == 0, "Invalid number of sweep directions for LSAO!");
    const int perSide = (numSweepDirs + 4) / 4 - 1; // side length of the square on which all directions lie -1
    const int halfPerSide = perSide / 2 + (perSide % 2);
    for(int i=0; i<perSide; ++i)
    {
      // Put opposing directions next to each other, so that a gather pass that doesn't sample all directions, only needs to sample an even number of directions to end up with non-negative occlusion.
      samplingDir[i*4 + 0] = ezVec2I32(i - halfPerSide, halfPerSide) * m_uiLineSamplePixelOffsetFactor; // Top
      samplingDir[i*4 + 1] = -samplingDir[i * 4 + 0]; // Bottom
      samplingDir[i*4 + 2] = ezVec2I32(halfPerSide, halfPerSide - i) * m_uiLineSamplePixelOffsetFactor; // Right
      samplingDir[i*4 + 3] = -samplingDir[i * 4 + 2]; // Left
    }

    // todo: Ddd debug test to check wheather any direction is duplicated. Mistakes in the equations above can easily happen!
  }

  for(int dirIndex = 0; dirIndex<EZ_ARRAY_SIZE(samplingDir); ++dirIndex)
  {
    ezUInt32 totalLineCountBefore = lineInstructions.GetCount();
    AddLinesForDirection(imageResolution, samplingDir[dirIndex], dirIndex, lineInstructions, totalNumberOfSamples);

    cb->Directions[dirIndex].Direction = ezVec2(static_cast<float>(samplingDir[dirIndex].x), static_cast<float>(samplingDir[dirIndex].y));
    cb->Directions[dirIndex].NumLines = lineInstructions.GetCount() - totalLineCountBefore;
    cb->Directions[dirIndex].LineInstructionOffset = totalLineCountBefore;
  }
  m_numSweepLines = lineInstructions.GetCount();
  cb->TotalLineNumber = m_numSweepLines;

  // Allocate and upload data structures to GPU
  {
    ezGALDevice* device = ezGALDevice::GetDefaultDevice();
    DestroyLineSweepData();

    // Output UAV for line sweep pass.
    // It is a texture to support FP16.
    {
      // TODO: Would need to use a texture if we want to use fp16
      ezGALBufferCreationDescription bufferDesc;
      bufferDesc.m_uiStructSize = 4; // fp32
      bufferDesc.m_uiTotalSize = 4 * totalNumberOfSamples;
      bufferDesc.m_BufferType = ezGALBufferType::Generic;
      bufferDesc.m_bUseForIndirectArguments = false;
      bufferDesc.m_bUseAsStructuredBuffer = false; // Just raw float32, nothing fancy
      bufferDesc.m_bAllowRawViews = false;
      bufferDesc.m_bStreamOutputTarget = false;
      bufferDesc.m_bAllowShaderResourceView = true;
      bufferDesc.m_bAllowUAV = true;
      bufferDesc.m_ResourceAccess.m_bReadBack = false;
      bufferDesc.m_ResourceAccess.m_bImmutable = false;

      m_hLineSweepOutputBuffer = device->CreateBuffer(bufferDesc);

      ezGALUnorderedAccessViewCreationDescription uavDesc;
      uavDesc.m_hBuffer = m_hLineSweepOutputBuffer;
      uavDesc.m_OverrideViewFormat = ezGALResourceFormat::RFloat;
      uavDesc.m_uiFirstElement = 0;
      uavDesc.m_uiNumElements = totalNumberOfSamples;
      uavDesc.m_bRawView = false;
      uavDesc.m_bAppend = false;
      m_hLineSweepOutputUAV = device->CreateUnorderedAccessView(uavDesc);

      ezGALResourceViewCreationDescription srvDesc;
      srvDesc.m_hBuffer = m_hLineSweepOutputBuffer;
      srvDesc.m_OverrideViewFormat = ezGALResourceFormat::RFloat;
      srvDesc.m_uiFirstElement = 0;
      srvDesc.m_uiNumElements = totalNumberOfSamples;
      srvDesc.m_bRawView = false;
      m_hLineSweepOutputSRV = device->CreateResourceView(srvDesc);
    }

    // Structured buffer per line.
    {
      ezGALBufferCreationDescription bufferDesc;
      bufferDesc.m_uiStructSize = sizeof(LineInstruction);
      bufferDesc.m_uiTotalSize = sizeof(LineInstruction) * m_numSweepLines;
      bufferDesc.m_BufferType = ezGALBufferType::Generic;
      bufferDesc.m_bUseForIndirectArguments = false;
      bufferDesc.m_bUseAsStructuredBuffer = true;
      bufferDesc.m_bAllowRawViews = false;
      bufferDesc.m_bStreamOutputTarget = false;
      bufferDesc.m_bAllowShaderResourceView = true;
      bufferDesc.m_bAllowUAV = false;
      bufferDesc.m_ResourceAccess.m_bReadBack = false;
      bufferDesc.m_ResourceAccess.m_bImmutable = true;

      m_hLineInfoBuffer = device->CreateBuffer(bufferDesc, ezArrayPtr<const ezUInt8>(reinterpret_cast<const ezUInt8*>(lineInstructions.GetData()), lineInstructions.GetCount() * sizeof(LineInstruction)));

      m_hLineSweepInfoSRV = device->GetDefaultResourceView(m_hLineInfoBuffer);
    }
  }

  m_bSweepDataDirty = false;
}

void ezScreenSpaceAmbientOcclusionPass::AddLinesForDirection(const ezVec2I32& imageResolution, const ezVec2I32& sampleDir, ezUInt32 lineIndex, ezDynamicArray<LineInstruction>& outinLineInstructions, ezUInt32& outinTotalNumberOfSamples)
{
  EZ_ASSERT_DEBUG(sampleDir.x != 0 || sampleDir.y != 0, "Sample direction is null (not pointing anywhere)");

  ezUInt32 firstNewLineInstructionIndex = outinLineInstructions.GetCount();

  // Always walk positive and flip if necessary later.
  ezVec2I32 walkDir(ezMath::Abs(sampleDir.x), ezMath::Abs(sampleDir.y));
  ezVec2 walkDirF(static_cast<float>(walkDir.x), static_cast<float>(walkDir.y));

  // Line "creation" always starts from 0,0 and walks along EITHER x or y depending which one is the less dominant axis.
  if (walkDir.x > walkDir.y)
  {
    // Dominant x, walk y
    for (ezInt32 y = imageResolution.y - 1; true; y -= m_uiLineToLinePixelOffset)
    {
      LineInstruction& newLine = outinLineInstructions.ExpandAndGetRef();
      float offset = ezMath::Round(HaltonSequence(lineIndex, y) * walkDir.x); // Pseudo random offset in x.
      newLine.FirstSamplePos = ezVec2(offset, static_cast<float>(y));

      // If we are already outside of the screen with x, this is not a point inside the screen!
      if (y < 0)
      {
        if (walkDir.y == 0)
          newLine.FirstSamplePos = ezVec2(static_cast<float>(imageResolution.x), static_cast<float>(imageResolution.y));
        else
        {
          ezVec2 floatStep = walkDirF * (static_cast<float>(-y) / walkDirF.y);
          newLine.FirstSamplePos += ezVec2(ezMath::Floor(floatStep.x + 0.5f), ezMath::Floor(floatStep.y + 0.5f));
        }

        // Left in y. We're done.
        if (newLine.FirstSamplePos.x >= imageResolution.x)
        {
          outinLineInstructions.PopBack();
          break;
        }
      }

      newLine.LineSweepOutputBufferOffset = outinTotalNumberOfSamples;

      // Compute how many samples this line will consume.
      unsigned int stepsToXBorder = static_cast<unsigned int>((imageResolution.x - newLine.FirstSamplePos.x) / walkDir.x + 1);
      unsigned int numSamples = 0;
      if (walkDir.y > 0)
      {
        unsigned int stepsToYBorder = static_cast<unsigned int>((imageResolution.y - newLine.FirstSamplePos.y) / walkDir.y + 1);
        numSamples = ezMath::Min(stepsToYBorder, stepsToXBorder);
      }
      else
        numSamples = stepsToXBorder;

      outinTotalNumberOfSamples += numSamples;
      newLine.LineDirIndex_NumSamples = lineIndex | (numSamples << 16);
    }
  }
  else
  {
    // Dominant y, walk x.
    for (ezInt32 x = imageResolution.x - 1; true; x -= m_uiLineToLinePixelOffset)
    {
      LineInstruction& newLine = outinLineInstructions.ExpandAndGetRef();
      float offset = ezMath::Round(HaltonSequence(lineIndex, x) * walkDir.y); // Pseudo random offset in y.
      newLine.FirstSamplePos = ezVec2(static_cast<float>(x), offset);

      // If we are already outside of the screen with x, this is not a point inside the screen!
      if (x < 0)
      {
        if (walkDir.x == 0)
          newLine.FirstSamplePos = ezVec2(static_cast<float>(imageResolution.x), static_cast<float>(imageResolution.y));
        else
        {
          ezVec2 floatStep = walkDirF * (static_cast<float>(-x) / walkDirF.x);
          newLine.FirstSamplePos += ezVec2(ezMath::Floor(floatStep.x + 0.5f), ezMath::Floor(floatStep.y + 0.5f));
        }

        // Left in y. We're done.
        if (newLine.FirstSamplePos.y >= imageResolution.y)
        {
          outinLineInstructions.PopBack();
          break;
        }
      }

      newLine.LineSweepOutputBufferOffset = outinTotalNumberOfSamples;

      // Compute how many samples this line will consume.
      unsigned int stepsToYBorder = static_cast<unsigned int>((imageResolution.y - newLine.FirstSamplePos.y) / walkDir.y + 1);
      unsigned int numSamples = 0;
      if (walkDir.x > 0)
      {
        unsigned int stepsToXBorder = static_cast<unsigned int>((imageResolution.x - newLine.FirstSamplePos.x) / walkDir.x + 1);
        numSamples = ezMath::Min(stepsToYBorder, stepsToXBorder);
      }
      else
        numSamples = stepsToYBorder;

      outinTotalNumberOfSamples += numSamples;
      newLine.LineDirIndex_NumSamples = lineIndex | (numSamples << 16);
    }
  }

  // Now consider x/y beeing negative.
  for (int c = 0; c < 2; ++c)
  {
    if (sampleDir.GetData()[c] < 0)
    {
      for (ezUInt32 i = firstNewLineInstructionIndex; i < outinLineInstructions.GetCount(); ++i)
      {
        outinLineInstructions[i].FirstSamplePos.GetData()[c] = imageResolution.GetData()[c] - 1 - outinLineInstructions[i].FirstSamplePos.GetData()[c];
      }
    }
  }

  // Validation.
#if EZ_ENABLED(EZ_COMPILE_FOR_DEBUG)
  for (ezUInt32 i = firstNewLineInstructionIndex; i < outinLineInstructions.GetCount(); ++i)
  {
    auto p = outinLineInstructions[i].FirstSamplePos;
    EZ_ASSERT_DEV(p.x >= 0 && p.y >= 0 && p.x < imageResolution.x && p.y < imageResolution.y, "First sweep line sample pos is invalid. Something is wrong with the sweep line generation algorithm.");
  }
#endif
}