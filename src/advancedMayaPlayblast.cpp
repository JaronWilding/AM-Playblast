#include "advancedMayaPlayblast.h"

// Maya includes
#include <maya/M3dView.h>
#include <maya/MArgDatabase.h>
#include <maya/MDoubleArray.h>
#include <maya/MGlobal.h>
#include <maya/MSyntax.h>

// Qt includes
#include <QtWidgets/QWidget.h>

// Boost includes
#include <boost/format.hpp>

//----------------------------------------------------
// STATIC CONSTANTS
//----------------------------------------------------

static const MString MEL_COMMAND = "AMPlayblast";

static const char* RESOLUTION_FLAG[2] = { "-r", "-resolution" };
static const char* VERSION_FLAG[2] = { "-v", "-version" };


//----------------------------------------------------
// PUBLIC METHODS
//----------------------------------------------------

AMPlayblastCmd::AMPlayblastCmd() : MPxCommand(), mUndoable(false), mResolution(false), mVersion(false) {} ;


MStatus AMPlayblastCmd::doIt(const MArgList& args) {
    MStatus status;

    // Parse arguments
    MArgDatabase argData(syntax(), args, &status);
    if (!status)
    {
        MGlobal::displayError("Error parsiing command arguments.");
        return(status);
    }

    mVersion = argData.isFlagSet(VERSION_FLAG[0]);
    mResolution = argData.isFlagSet(RESOLUTION_FLAG[0]);
   

    // Primary workload.

    MGlobal::displayInfo("Starting rendering...");
    boost::format fmt = boost::format("Width: %1% | Height: %2%");

    // Get our variables and set them.
    QWidget &view_window = *M3dView::active3dView().widget();
    Vector2 resolution = Vector2(view_window.size().width(), view_window.size().height());
    mOriginalResolution, mRenderResolution = resolution;

    if (mResolution)
    {
        mRenderResolution = Vector2(argData.flagArgumentDouble(RESOLUTION_FLAG[0], 0),
                                    argData.flagArgumentDouble(RESOLUTION_FLAG[0], 1));
    }

    // Display our current resolution
    MGlobal::displayInfo(boost::str(fmt % resolution.x % resolution.y).c_str());


    view_window.setHidden(true);

    view_window.resize(mRenderResolution.x, mRenderResolution.y);
    view_window.repaint(0, 0, mRenderResolution.x, mRenderResolution.y);

    MGlobal::displayInfo(boost::str(fmt % view_window.size().width() % view_window.size().height()).c_str());

    view_window.resize(resolution.x, resolution.y);
    view_window.repaint(0, 0, resolution.x, resolution.y);

    view_window.setHidden(false);

    MGlobal::displayInfo("Finished rendering...");
    
    //return status;
    return(redoIt());
}

MStatus AMPlayblastCmd::redoIt()
{
    return MS::kSuccess;
}

bool AMPlayblastCmd::isUndoable() const
{
    return false;
}

//----------------------------------------------------
// STATIC METHODS
//----------------------------------------------------

void* AMPlayblastCmd::Creator() {
    return new AMPlayblastCmd();
}

MString AMPlayblastCmd::CommandName() {
    return MEL_COMMAND;
}

MSyntax AMPlayblastCmd::CreateSyntax() {
    MSyntax syntax;

    syntax.enableEdit(true);
    syntax.enableQuery(true);

    syntax.addFlag(RESOLUTION_FLAG[0], RESOLUTION_FLAG[1], MSyntax::kDouble, MSyntax::kDouble);
    syntax.addFlag(VERSION_FLAG[0], VERSION_FLAG[1]);

    return syntax;
}