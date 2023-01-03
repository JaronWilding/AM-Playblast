//-
// ==========================================================================
// Copyright 2017 Autodesk, Inc. All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// ==========================================================================
//+
// Example Plugin: captureViewRenderCmd.cpp
//
//
// This is a plugin to capture a range of frames from modelPanel4 and show a
// specified frame. Output color transform can be enabled/disabled when capturing.
//
// loadPlugin captureViewRenderCmd;
//
// captureViewRender -captureFrame 1 10 true;
// captureViewRender -showFrame 6;
// captureViewRender -captureFrame 1 10 false;
// captureViewRender -showFrame 6;
// 
// 
//

#include "getMayaRenderData.h"
#include <maya/MApiNamespace.h>
#include <maya/MArgDatabase.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>
#include <maya/MAnimControl.h>
#include <maya/MTime.h>
#include <maya/MRenderTargetManager.h>
#include <maya/MViewport2Renderer.h>
#include <vector>
#define kCaptureFrame "-cf"
#define kCaptureFrameLong "-captureFrame"
#define kShowFrame "-sf"
#define kShowFrameLong "-showFrame"



std::vector<MayaViewportRenderData*> gRenderData;
class captureViewRenderCmd : public MPxCommand
{
public:
    captureViewRenderCmd()
    {
    }
    ~captureViewRenderCmd() override
    {
    }
    static void* creator()
    {
        return new captureViewRenderCmd;
    }
    static MSyntax newSyntax()
    {
        MSyntax syntax;
        syntax.addFlag(kCaptureFrame, kCaptureFrameLong,
            MSyntax::kTime /*startTime*/,
            MSyntax::kTime /*endTime*/,
            MSyntax::kBoolean /*outputColorTransform*/);
        syntax.addFlag(kShowFrame, kShowFrameLong, MSyntax::kTime);
        syntax.enableQuery(false);
        syntax.enableEdit(false);
        return syntax;
    }
    MStatus doIt(const MArgList&) override;
};
MStatus captureViewRenderCmd::doIt(const MArgList& args)
{
    MStatus status = MStatus::kFailure;
    MArgDatabase argData(syntax(), args, &status);
    if (!status)
        return status;
    if (argData.isFlagSet(kCaptureFrame))
    {
        // Remove all previous captured frames
        for (unsigned int i = 0; i < gRenderData.size(); i++)
        {
            delete gRenderData[i];
        }
        gRenderData.clear();
        MTime startTime;
        MTime endTime;
        bool outputColorTransform = false;
        argData.getFlagArgument(kCaptureFrame, 0, startTime);
        argData.getFlagArgument(kCaptureFrame, 1, endTime);
        argData.getFlagArgument(kCaptureFrame, 2, outputColorTransform);
        for (MTime currentTime = startTime; currentTime <= endTime;
            currentTime++)
        {
            // schedule idle render tasks
            MayaViewportRenderData* newRenderTask =
                new MayaViewportRenderData(currentTime, outputColorTransform);
            gRenderData.push_back(newRenderTask);
            MGlobal::executeTaskOnIdle(MayaViewportRenderData::doRenderTask,
                newRenderTask,
                MGlobal::kHighIdlePriority);
        }
    }
    else if (argData.isFlagSet(kShowFrame))
    {
        MTime time;
        argData.getFlagArgument(kShowFrame, 0, time);
        // Find the frame in the render data list
        unsigned int frameToShow = gRenderData.size();
        for (unsigned int i = 0; i < gRenderData.size(); i++)
        {
            if (time == gRenderData[i]->renderTime())
            {
                frameToShow = i;
                break;
            }
        }
        if (frameToShow == gRenderData.size())
        {
            MGlobal::displayError("No captured frame found!");
            return MS::kFailure;
        }
        if (!gRenderData[frameToShow]->isDone())
        {
            MGlobal::displayError("The frame is not captured yet!");
            return MS::kFailure;
        }
        // Copy the captured target to screen.
        MRenderer::theRenderer()->copyTargetToScreen(
            gRenderData[frameToShow]->renderTarget());
    }
    return MS::kSuccess;
}
MStatus initializePlugin(MObject _obj)
{
    MFnPlugin plugin(_obj, PLUGIN_COMPANY, "1.0");
    MStatus stat = plugin.registerCommand("captureViewRender",
        captureViewRenderCmd::creator,
        captureViewRenderCmd::newSyntax);
    if (!stat)
        stat.perror("registerCommand");
    return stat;
}
MStatus uninitializePlugin(MObject _obj)
{
    MFnPlugin plugin(_obj);
    MStatus stat = plugin.deregisterCommand("captureViewRender");
    if (!stat)
        stat.perror("deregisterCommand");
    // Remove all captured frames' render data
    for (unsigned int i = 0; i < gRenderData.size(); i++)
    {
        delete gRenderData[i];
    }
    gRenderData.clear();
    return stat;
}


*/