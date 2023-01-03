#pragma once

#include <maya/MGlobal.h>
#include <maya/MTime.h>


class MayaViewportRenderData
{
public:
    MayaViewportRenderData(const MTime& renderTime, bool outputColorTransform);
    ~MayaViewportRenderData();
    static void doRenderTask(void* clientData);
    const MTime& renderTime() const;
    const MHWRender::MRenderTarget* renderTarget() const;
    bool isDone() const;
private:
    MHWRender::MRenderTarget* mTarget;
    MTime mRenderTime;
    bool mOutputColorTransform;
    bool mIsDone;
};