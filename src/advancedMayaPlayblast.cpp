// -----------------------------------------
//
// advancedMayaPlayblast.cpp
// AdvancedMayaPlayblaster
//
// Created by Jaron Wilding on 20/12/2022
//
// -----------------------------------------

// Program includes
#include "advancedMayaPlayblast.h"
#include "ffConvert.h"

// Maya includes
#include <maya/M3dView.h>
#include <maya/MAnimControl.h>
#include <maya/MArgDatabase.h>
#include <maya/MDoubleArray.h>
#include <maya/MGlobal.h>
#include <maya/MImage.h>
#include <maya/MSyntax.h>


#include "hwApiTextureTestStrings.h"



// Boost includes
#include <boost/format.hpp>
#include <sstream>


// -----------------------------------------
// General methods
// -----------------------------------------

advancedPlayblastCmd::advancedPlayblastCmd() : MPxCommand(), mUndoable(false), mResolution(false), mFrameRange(false), mHelp(false) {
    fRenderer = MHWRender::MRenderer::theRenderer();
};

advancedPlayblastCmd::~advancedPlayblastCmd() {};

void* advancedPlayblastCmd::creator() {
    return new advancedPlayblastCmd();
}

bool advancedPlayblastCmd::isUndoable() const
{
    return false;
}


//----------------------------------------------------
// Syntax definition
//----------------------------------------------------

#define kHelp               "-h"
#define kHelpLong           "-help"

#define kFrameRange         "-f"
#define kFrameRangeLong     "-frames"
#define kOutputDir          "-o"
#define kOutputDirLong      "-output"
#define kOutputType         "-t"
#define kOutputTypeLong     "-type"
#define kResolution         "-r"
#define kResolutionLong     "-resolution"

const MString advancedPlayblastCmd::kName("AMPlayblast");


MSyntax advancedPlayblastCmd::createSyntax() {
    MSyntax syntax;

    syntax.enableQuery(true);

    syntax.addFlag(kHelp, kHelpLong);
    
    syntax.addFlag(kFrameRange, kFrameRangeLong, MSyntax::kDouble, MSyntax::kDouble);
    syntax.addFlag(kOutputDir, kOutputDirLong, MSyntax::kString);
    syntax.addFlag(kOutputType, kOutputTypeLong, MSyntax::kString);
    syntax.addFlag(kResolution, kResolutionLong, MSyntax::kDouble, MSyntax::kDouble);

    return syntax;
}

MStatus advancedPlayblastCmd::parseArgs(const MArgList& args, MStatus& status)
{
    MArgDatabase argData(syntax(), args, &status);

    if (!status)
    {
        MGlobal::displayError("Error parsing command arguments.");
        return status;
    }
        
    mHelp = argData.isFlagSet(kHelp);
    
    mFrameRange = argData.isFlagSet(kFrameRange);
    mOutputDir = argData.isFlagSet(kOutputDir);
    mOutputType = argData.isFlagSet(kOutputType);
    mResolution = argData.isFlagSet(kResolution);

    if (mFrameRange) {
        vFrameRange = Vector2(MTime(argData.flagArgumentDouble(kFrameRange, 0)), MTime(argData.flagArgumentDouble(kFrameRange, 1)));
    }

    if (mOutputDir) {
        outputDir = argData.flagArgumentString(kOutputDir, 0);
    }

    if (mOutputType) {
        outputType = argData.flagArgumentString(kOutputType, 0);
    }

    if (mResolution) {
        vRenderResolution = Vector2(argData.flagArgumentDouble(kResolution, 0), argData.flagArgumentDouble(kResolution, 1));
    }

    return status;
}


//----------------------------------------------------
// Maya default Methods
//----------------------------------------------------

MStatus advancedPlayblastCmd::doIt(const MArgList& args) {
    MStatus status;

    status = parseArgs(args, status);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return redoIt();
}


MStatus advancedPlayblastCmd::redoIt() {

    if (mHelp) {
        MGlobal::displayInfo("Help text goes here!");
        MGlobal::displayInfo(outputType);
        MGlobal::displayInfo(outputDir);
        return MStatus::kSuccess;
    }

    //renderFile();
    test();
    return MStatus::kSuccess;
}

//----------------------------------------------------
// Main functionality
//----------------------------------------------------


void advancedPlayblastCmd::test() {

    MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
    if (theRenderer) {
        const MHWRender::MRenderTargetManager* targetManager = theRenderer->getRenderTargetManager();
        if (!targetManager)
            MGlobal::displayInfo("Couldn't get target manager");

        mTarget = targetManager->acquireRenderTargetFromScreen(MString("CapturedTarget") + mRenderTime.value());
    }

    MayaViewportRenderData



}

void advancedPlayblastCmd::renderFile() {
    // Primary workload.

    MGlobal::displayInfo("Starting rendering...");
    boost::format res_fmt = boost::format("Width: %1% | Height: %2%");
    boost::format time_fmt = boost::format("Min: %1% | Max: %2%");
    boost::format ctime_fmt = boost::format("Frame: %1%");
    boost::format output_fmt = boost::format("%04i");

    // Get our variables and set them.
    M3dView viewer = M3dView::active3dView();
    QWidget& window = *viewer.widget();
    vOriginalResolution = Vector2(window.size().width(), window.size().height());
    vRenderResolution = mResolution ? vRenderResolution : vOriginalResolution;
    
    // Display our current resolution
    MGlobal::displayInfo(boost::str(res_fmt % vOriginalResolution.x % vOriginalResolution.y).c_str());

    // Resize and Repaint view
    repaintView(window, vRenderResolution, true);

    vFrameRange = mFrameRange ? vFrameRange : Vector2(MAnimControl::minTime(), MAnimControl::maxTime());

    
    const MHWRender::MRenderTargetManager* targetManager = fRenderer->getRenderTargetManager();
    MHWRender::MTextureManager* textureManager = fRenderer->getTextureManager();

    MHWRender::MRenderTargetDescription targetDesc = MHWRender::MRenderTargetDescription();
    
    for (double ii = vFrameRange.x; ii <= vFrameRange.y; ii++) {
        viewer.refresh();
        MAnimControl::setCurrentTime(MTime(ii));
        
        std::stringstream output_file;
        output_file << outputDir << "/" << boost::str(output_fmt % ii)  << ".png";
       
        MGlobal::displayInfo("1");
        MHWRender::MRenderTarget& renderTarget = *targetManager->acquireRenderTarget(targetDesc);
        MGlobal::displayInfo("2");
        // Get the render target data (the screen pixels)
        int rowPitch = 0;
        size_t slicePitch = 0;
        MGlobal::displayInfo(renderTarget.className());
        renderTarget.rawData(rowPitch, slicePitch);
        MGlobal::displayInfo("3");
        //targetManager->releaseRenderTarget(renderTarget);
        MGlobal::displayInfo("4");

        /*

        MHWRender::MTexture* texture = NULL;
        if (targetData != NULL) {
            MHWRender::MTextureDescription textureDesc;
            textureDesc.fWidth = targetDesc.width();
            textureDesc.fHeight = targetDesc.height();
            textureDesc.fDepth = 1;
            textureDesc.fBytesPerRow = rowPitch;
            textureDesc.fBytesPerSlice = slicePitch;
            textureDesc.fMipmaps = 1;
            textureDesc.fArraySlices = targetDesc.arraySliceCount();
            textureDesc.fFormat = targetDesc.rasterFormat();
            textureDesc.fTextureType = MHWRender::kImage2D;
            textureDesc.fEnvMapType = MHWRender::kEnvNone;

            texture = textureManager->acquireTexture("textureTest", textureDesc, targetData);
        }

        MImage img = MImage();
        texture->update(img, false);
        //img.create(viewer.portWidth(), viewer.portHeight(), 4);

        */

        MGlobal::displayInfo(output_file.str().c_str());
        //MGlobal::displayInfo(boost::str(ctime_fmt % ii).c_str());
    }

    
    

    MGlobal::displayInfo(boost::str(time_fmt % vFrameRange.x % vFrameRange.y).c_str());

    // Resize and Repaint view
    repaintView(window, vOriginalResolution, false);

    MGlobal::displayInfo("Finished rendering...");
}

Vector2 advancedPlayblastCmd::repaintView(QWidget &view_window, Vector2 resolution, bool hidden) {
    view_window.setHidden(hidden);
    view_window.resize(resolution.x, resolution.y);
    view_window.repaint(0, 0, resolution.x, resolution.y);
    return Vector2(view_window.size().width(), view_window.size().height());
}

