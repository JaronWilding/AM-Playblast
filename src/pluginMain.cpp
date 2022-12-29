// -----------------------------------------
//
// pluginMain.cpp
// AdvancedMayaPlayblaster
//
// Created by Jaron Wilding on 20/12/2022
//
// -----------------------------------------

#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>



#include "advancedMayaPlayblast.h"

MStatus initializePlugin(MObject pluginObj) {

    MStatus status;
    MFnPlugin pluginFn(pluginObj, "Jaron Wilding", kVersion, "Any", &status);

    status = pluginFn.registerCommand(advancedPlayblastCmd::kName, advancedPlayblastCmd::creator, advancedPlayblastCmd::createSyntax);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return MS::kSuccess;
}


MStatus uninitializePlugin(MObject pluginObj) {
    MStatus status;

    MFnPlugin fnPlugin(pluginObj);

    status = fnPlugin.deregisterCommand(advancedPlayblastCmd::kName);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return MS::kSuccess;
}
