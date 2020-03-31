/*
 * Copyright (C) 2019 GlobalLogic
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

#define LOG_TAG "android.hardware.graphics.allocator@3.0-impl"

#include "Allocator.h"
#include "GrallocLoader.h"

namespace android {
namespace hardware {
namespace graphics {
namespace allocator {
namespace V3_0 {
namespace renesas {
namespace hal {
namespace detail {

Return<void> Allocator::dumpDebugInfo(IAllocator::dumpDebugInfo_cb _hidl_cb) {
    _hidl_cb(mHal->dumpDebugInfo());
    return Void();
}

Return<void> Allocator::allocate(const BufferDescriptor& descriptor, uint32_t count,
                      IAllocator::allocate_cb _hidl_cb) {
    uint32_t stride = 0;
    std::vector<const native_handle_t*> buffers;

    std::unique_lock<std::mutex> lock(mLock);

    Error error = mHal->allocateBuffers(descriptor, count, &stride, &buffers);
    if (error != Error::NONE) {
        _hidl_cb(error, 0, hidl_vec<hidl_handle>());
        return Void();
    }

    hidl_vec<hidl_handle> hidlBuffers(buffers.cbegin(), buffers.cend());
    _hidl_cb(Error::NONE, stride, hidlBuffers);

    // free the local handles
    mHal->freeBuffers(buffers);

    return Void();
}

}  // namespace detail

extern "C" IAllocator* HIDL_FETCH_IAllocator(const char* /* name */) {
    return passthrough::GrallocLoader::load();
}

}  // namespace hal
}  // namespace renesas
}  // namespace V3_0
}  // namespace allocator
}  // namespace graphics
}  // namespace hardware
}  // namespace android
