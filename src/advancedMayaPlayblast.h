#pragma once

#include <maya/MPxCommand.h>
#include <maya/MVector.h>

#include "Vector2.h"

class AMPlayblastCmd : public MPxCommand {
public:
	AMPlayblastCmd();
	virtual ~AMPlayblastCmd() {};

	virtual MStatus doIt(const MArgList& args);
	virtual MStatus redoIt() override;
	virtual bool isUndoable() const override;

	// Static methods
	static void* Creator();
	static MString CommandName();
	static MSyntax CreateSyntax();

private:
	bool mUndoable;

	bool mResolution;
	bool mVersion;

	Vector2 mOriginalResolution;
	Vector2 mRenderResolution;
};

