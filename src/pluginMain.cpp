#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

#include "advancedMayaPlayblast.h"

MStatus initializePlugin(MObject pluginObj) {
    const char* vendor = "Jaron Wilding";
    const char* version = "1.0.0";
    const char* requiredApiVersion = "Any";

    MStatus status;
    MFnPlugin pluginFn(pluginObj, vendor, version, requiredApiVersion, &status);

    status = pluginFn.registerCommand(AMPlayblastCmd::CommandName(), AMPlayblastCmd::Creator, AMPlayblastCmd::CreateSyntax);
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return MS::kSuccess;
}


MStatus uninitializePlugin(MObject pluginObj) {
    MStatus status;

    MFnPlugin fnPlugin(pluginObj);

    status = fnPlugin.deregisterCommand(AMPlayblastCmd::CommandName());
    CHECK_MSTATUS_AND_RETURN_IT(status);

    return MS::kSuccess;
}
