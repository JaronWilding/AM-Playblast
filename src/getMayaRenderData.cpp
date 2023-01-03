
#include "getMayaRenderData.h"

#include <maya/MAnimControl.h>
#include <maya/MRenderTargetManager.h>
#include <maya/MViewport2Renderer.h>

MayaViewportRenderData::MayaViewportRenderData(const MTime & renderTime, bool outputColorTransform) : mRenderTime(renderTime), mOutputColorTransform(outputColorTransform)
{
    MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
    if (theRenderer)
    {
        const MHWRender::MRenderTargetManager* targetManager = theRenderer->getRenderTargetManager();
        if (!targetManager)
            return;
        mTarget = targetManager->acquireRenderTargetFromScreen(MString("CapturedTarget") + mRenderTime.value());
    }
    mIsDone = false;
}

MayaViewportRenderData::~MayaViewportRenderData()
{
    MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
    if (theRenderer)
    {
        const MHWRender::MRenderTargetManager* targetManager = theRenderer->getRenderTargetManager();
        if (targetManager)
        {
            targetManager->releaseRenderTarget(mTarget);
        }
    }
}


void MayaViewportRenderData::doRenderTask(void* clientData)
{
    MayaViewportRenderData* renderData = static_cast<MayaViewportRenderData*>(clientData);
    if (!renderData)
        return;

    MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
    if (!theRenderer)
    {
        return;
    }
    MHWRender::MRenderTarget*& target = renderData->mTarget;
    if (!target)
    {
        return;
    }
    MAnimControl::setCurrentTime(renderData->mRenderTime);

    // Turn on/off output color transform for renderer and playblast as required
    static const MString cmQR("colorManagementPrefs -q -outputTarget renderer -outputTransformEnabled");
    static const MString cmER("colorManagementPrefs -e -outputTarget renderer -inhibitEvents -outputTransformEnabled ");
    static const MString cmQP("colorManagementPrefs -q -outputTarget playblast -outputTransformEnabled");
    static const MString cmEP("colorManagementPrefs -e -outputTarget playblast -inhibitEvents -outputTransformEnabled ");


    int result = 0;
    MGlobal::executeCommand(cmQR, result);
    bool outputColorTransformR = (result != 0);
    if (outputColorTransformR != renderData->mOutputColorTransform)
    {
        MString cmd = cmER + (renderData->mOutputColorTransform ? 1 : 0);
        MGlobal::executeCommand(cmd);
    }
    MGlobal::executeCommand(cmQP, result);
    bool outputColorTransformP = (result != 0);
    if (outputColorTransformP != renderData->mOutputColorTransform)
    {
        MString cmd = cmEP + (renderData->mOutputColorTransform ? 1 : 0);
        MGlobal::executeCommand(cmd);
    }

    theRenderer->render("modelPanel4", &target, 1);
    // Restore output color transform for renderer and playblast
    if (outputColorTransformR != renderData->mOutputColorTransform)
    {
        MString cmd = cmER + (outputColorTransformR ? 1 : 0);
        MGlobal::executeCommand(cmd);
    }
    if (outputColorTransformP != renderData->mOutputColorTransform)
    {
        MString cmd = cmEP + (outputColorTransformP ? 1 : 0);
        MGlobal::executeCommand(cmd);
    }
    renderData->mIsDone = true;
}


const MTime& MayaViewportRenderData::renderTime() const
{
    return mRenderTime;
}
const MHWRender::MRenderTarget* MayaViewportRenderData::renderTarget() const
{
    return mTarget;
}
bool MayaViewportRenderData::isDone() const
{
    return mIsDone;
}
