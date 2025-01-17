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

#ifndef ANDROID_SURFACE_TEXTURE_LAYER_H
#define ANDROID_SURFACE_TEXTURE_LAYER_H

#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>

#include <utils/Errors.h>
#include <gui/BufferQueue.h>

namespace android {
// ---------------------------------------------------------------------------

class Layer;

// SurfaceTextureLayer is now a BufferQueue since SurfaceTexture has been
// refactored
class SurfaceTextureLayer : public BufferQueue
{
public:
    SurfaceTextureLayer();
    ~SurfaceTextureLayer();

    virtual status_t connect(int api, QueueBufferOutput* output);

    // *** MediaTek ******************************************************* //
public:
    // for get real type
    virtual int32_t getType() const {
        return BufferQueue::TYPE_SurfaceTextureLayer;
    }

    // for buffer log
    virtual status_t queueBuffer(int buf,
                                 const QueueBufferInput& input,
                                 QueueBufferOutput* output);
    // ******************************************************************** //
};

// ---------------------------------------------------------------------------
}; // namespace android

#endif // ANDROID_SURFACE_TEXTURE_LAYER_H
