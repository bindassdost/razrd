/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>

#include <utils/Errors.h>
#include <utils/Log.h>
#include <cutils/xlog.h>

#include <ui/GraphicBuffer.h>

#include "LayerScreenshot.h"
#include "SurfaceFlinger.h"
#include "DisplayHardware/DisplayHardware.h"


namespace android {
// ---------------------------------------------------------------------------

LayerScreenshot::LayerScreenshot(SurfaceFlinger* flinger, DisplayID display,
        const sp<Client>& client)
    : LayerBaseClient(flinger, display, client),
      mTextureName(0), mFlinger(flinger)
{
    // *** MediaTek ******************************************************* //
    Mutex::Autolock _l(mLock);
    mLayers.add(this);
    XLOGI("[%s] create:%p, count:%d", __func__, this, mLayers.size());
    // ******************************************************************** //
}

LayerScreenshot::~LayerScreenshot()
{
    if (mTextureName) {
        mFlinger->postMessageAsync(
                new SurfaceFlinger::MessageDestroyGLTexture(mTextureName) );
    }

    // *** MediaTek ******************************************************* //
    Mutex::Autolock _l(mLock);
    uint32_t i;
    for (i = 0; i < mLayers.size(); i++) {
        if (this == mLayers[i]) {
            mLayers.removeAt(i);
            break;
        }
    }    
    XLOGI("[%s] remove:%p, count:%d", __func__, this, mLayers.size());
    // ******************************************************************** //
}

status_t LayerScreenshot::captureLocked() {
    GLfloat u, v;
    status_t result = mFlinger->renderScreenToTextureLocked(0, &mTextureName, &u, &v);
    if (result != NO_ERROR) {
        return result;
    }
    initTexture(u, v);
    return NO_ERROR;
}

status_t LayerScreenshot::capture() {
    GLfloat u, v;
    status_t result = mFlinger->renderScreenToTexture(0, &mTextureName, &u, &v);
    if (result != NO_ERROR) {
        return result;
    }
    initTexture(u, v);
    return NO_ERROR;
}

void LayerScreenshot::initTexture(GLfloat u, GLfloat v) {
    glBindTexture(GL_TEXTURE_2D, mTextureName);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    mTexCoords[0] = 0;         mTexCoords[1] = v;
    mTexCoords[2] = 0;         mTexCoords[3] = 0;
    mTexCoords[4] = u;         mTexCoords[5] = 0;
    mTexCoords[6] = u;         mTexCoords[7] = v;
}

void LayerScreenshot::initStates(uint32_t w, uint32_t h, uint32_t flags) {
    LayerBaseClient::initStates(w, h, flags);
    if (!(flags & ISurfaceComposer::eHidden)) {
        capture();
    }
}

uint32_t LayerScreenshot::doTransaction(uint32_t flags)
{
    const Layer::State& draw(drawingState());
    const Layer::State& curr(currentState());

    if (draw.flags & ISurfaceComposer::eLayerHidden) {
        if (!(curr.flags & ISurfaceComposer::eLayerHidden)) {
            // we're going from hidden to visible
            status_t err = captureLocked();
            if (err != NO_ERROR) {
                ALOGW("createScreenshotSurface failed (%s)", strerror(-err));
            }
        }
    } else if (curr.flags & ISurfaceComposer::eLayerHidden) {
        // we're going from visible to hidden
        if (mTextureName) {
            glDeleteTextures(1, &mTextureName);
            mTextureName = 0;
        }
    }
    return LayerBaseClient::doTransaction(flags);
}

void LayerScreenshot::onDraw(const Region& clip) const
{
    const State& s(drawingState());
    if (s.alpha>0) {
        const DisplayHardware& hw(graphicPlane(0).displayHardware());
        const GLfloat alpha = s.alpha/255.0f;
        const uint32_t fbHeight = hw.getHeight();

        if (s.alpha == 0xFF) {
            glDisable(GL_BLEND);
        } else {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        // [mtk] re-cal coords for case with using hwrotation
        //--------------------------------------------------------------------
        GLfloat tmpVertices[4][2];
        Transform orientationTransform;
        int w = graphicPlane(0).getDisplayWidth();
        int h = graphicPlane(0).getDisplayHeight();
        int reverseDisplayOrientation = (4 - graphicPlane(0).getDisplayOrientation()) % 4; // reverse transform trick
    
        GraphicPlane::orientationToTransfrom(
            reverseDisplayOrientation, w, h, &orientationTransform);
        const Transform tr(mTransform * orientationTransform);

        w = hw.getWidth();
        h = hw.getHeight();
        tr.transform(tmpVertices[0], 0, 0);
        tr.transform(tmpVertices[1], 0, h);
        tr.transform(tmpVertices[2], w, h);
        tr.transform(tmpVertices[3], w, 0);
        for (size_t i = 0; i < 4; i++) {
            tmpVertices[i][1] = fbHeight - tmpVertices[i][1];
        }
        //--------------------------------------------------------------------

        glColor4f(0, 0, 0, alpha);

        glDisable(GL_TEXTURE_EXTERNAL_OES);
        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, mTextureName);
        glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, mTexCoords);
        glVertexPointer(2, GL_FLOAT, 0, tmpVertices);      // [mtk] use adjusted coord
        glDrawArrays(GL_TRIANGLE_FAN, 0, mNumVertices);

        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}

// ---------------------------------------------------------------------------

}; // namespace android
