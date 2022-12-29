// -----------------------------------------
//
// advancedMayaPlayblast.cpp
// AdvancedMayaPlayblaster
//
// Created by Jaron Wilding on 20/12/2022
//
// -----------------------------------------

#pragma once

// Program includes
#include "Vector2.h"

// Maya includes
#include <maya/MPxCommand.h>
#include <maya/MVector.h>
#include <maya/MTextureManager.h>
#include <maya/MViewport2Renderer.h>
#include <maya/MRenderTargetManager.h>

// QT includes
#include <QtWidgets/QWidget.h>


//----------------------------------------------------
// Consts definition
//----------------------------------------------------

static const char* kVersion = "1.0.0";


// -----------------------------------------
// Main tool
// -----------------------------------------

class advancedPlayblastCmd : public MPxCommand {
public:
	advancedPlayblastCmd();
	virtual ~advancedPlayblastCmd();
	
	static void* creator();
	static MSyntax createSyntax();

	MStatus parseArgs(const MArgList& args, MStatus& status);

	virtual MStatus doIt(const MArgList& args);
	virtual MStatus redoIt() override;

	virtual bool isUndoable() const override;

	// Custom Methods
	void renderFile();
	void test();
	Vector2 repaintView(QWidget &view_window, Vector2 resolution, bool hidden = false);

	// Static variables
	static const MString kName;
	static const std::string kVersion;

private:
	bool mHelp;
	bool mUndoable;

	bool mFrameRange;
	bool mOutputDir;
	bool mOutputType;
	bool mResolution;

	MHWRender::MRenderTarget* mTarget;
	MTime mRenderTime;

	Vector2 vOriginalResolution;
	Vector2 vRenderResolution;
	Vector2 vFrameRange;
	MString outputDir;
	MString outputType;

protected:
	MHWRender::MRenderer* fRenderer;
};

