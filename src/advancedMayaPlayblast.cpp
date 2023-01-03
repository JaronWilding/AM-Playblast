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
#include <maya/MRenderUtilities.h>
#include <maya/MDrawContext.h>


#include "hwApiTextureTestStrings.h"
#include "getMayaRenderData.h"



// Boost includes
#include <boost/format.hpp>
#include <sstream>


// -----------------------------------------
// General methods
// -----------------------------------------

advancedPlayblastCmd::advancedPlayblastCmd() : MPxCommand(), mUndoable(false), mResolution(false), mFrameRange(false), mHelp(false) {
    MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
    const MHWRender::MRenderTargetManager* targetManager = theRenderer->getRenderTargetManager();
    mTarget = targetManager->acquireRenderTargetFromScreen(MString("CapturedTarget") + mRenderTime.value());
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
std::vector<MayaViewportRenderData*> gRenderData;
std::vector<MTexture*> textures;
std::vector<std::vector<unsigned char>> dataImages;


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
    M3dView viewer = M3dView::active3dView();
    MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
    const MHWRender::MRenderTargetManager* targetManager = theRenderer->getRenderTargetManager();
    MHWRender::MTextureManager* textureManager = theRenderer->getTextureManager();

    vFrameRange = mFrameRange ? vFrameRange : Vector2(MAnimControl::minTime(), MAnimControl::maxTime());

    for (MTime startFrame = vFrameRange.mX; startFrame <= vFrameRange.mY; startFrame++) {

        // Set the specific time
        MAnimControl::setCurrentTime(startFrame);
        viewer.refresh();

        // Create our render target
        MHWRender::MRenderTarget* viewportTarget = targetManager->acquireRenderTargetFromScreen("viewport");
        
        
        // Create a render target descriptor
        //MHWRender::MRenderTargetDescription targetDesc;
        //viewportTarget->targetDescription(targetDesc);

        MDrawContext* context = MHWRender::MRenderUtilities::acquireSwatchDrawContext(viewportTarget);
        MTextureManager *textureManager = theRenderer->getTextureManager();
        MTexture* colourTexture = context->copyCurrentColorRenderTargetToTexture();
        colourTexture->setHasTransparentAlpha(false);
        MTextureDescription colourDescription;
        colourTexture->textureDescription(colourDescription);
        int rowPitch = 0;
        size_t slicePitch = 0;

        // fHeight      = 732
        // fWidth       = 529
        // rowPitch     = 8464
        // slicePitch   = 6195648



        unsigned char* pixelData = (unsigned char*)(colourTexture->rawData(rowPitch, slicePitch));
        /*
        unsigned char* pixelVal = NULL;
        for (unsigned int ii = 0; ii < colourDescription.fHeight; ii++) {
            pixelVal = (pixelData + (ii * rowPitch)); // pixelVal is given location of pixelData.
            for (unsigned jj = 0; jj < colourDescription.fWidth; jj++) {
                unsigned char val = *pixelVal;
                pixelVal++;
            }
        }
        */
        colourTexture->freeRawData(pixelData);
        targetManager->releaseRenderTarget(viewportTarget);

        boost::format res_fmt = boost::format("C:/Users/Jaron/Desktop/test/Image_%04d.png");


        textureManager->saveTexture(colourTexture, MString(boost::str(res_fmt % startFrame.value()).c_str()));

        //break;

            

        /*

        // Render our viewport to the target.
        theRenderer->render("viewport", &viewportTarget, 1);

        int rowPitch = 0;
        size_t slicePitch = 0;
        unsigned char* pixelData = (unsigned char*)(viewportTarget->rawData(rowPitch, slicePitch));
        
        std::vector<unsigned char> current_image;
        unsigned char* val = NULL;
        int total_len = 0;
        for (unsigned int i = 0; i < targetDesc.height(); i++)
        {
            val = (pixelData + (i * rowPitch));
            for (unsigned int j = 0; j < targetDesc.width(); j++) {
                total_len++;
                val++;
            }
        }

        */
        
        // Cleanup the pixel data, and release our render target.
        //viewportTarget->freeRawData(pixelData);
        
        

        // Release the target else errors!
    }
    //}

    displayInfo("Render finished");


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

    
    
    for (double startFrame = vFrameRange.x; startFrame <= vFrameRange.y; startFrame++) {
        viewer.refresh();
        MTime currentFrame = MTime(startFrame);





        //MayaViewportRenderData* newRenderTask = new MayaViewportRenderData(currentFrame, false);
        //gRenderData.push_back(newRenderTask);
        //MGlobal::execute
        //MGlobal::executeTaskOnIdle(MayaViewportRenderData::doRenderTask, newRenderTask, MGlobal::kHighIdlePriority);
        //MAnimControl::setCurrentTime(currentFrame);
        
        //std::stringstream output_file;
        //output_file << outputDir << "/" << boost::str(output_fmt % ii)  << ".png";
       

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

