/*
 * Copyright (C) 2012 The Android Open Source Project
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

#include <utils/Errors.h>

#ifndef CRYPTO_API_H_

#define CRYPTO_API_H_

namespace android {

struct AString;
struct CryptoPlugin;

struct CryptoFactory {
    CryptoFactory() {}
    virtual ~CryptoFactory() {}

    virtual bool isCryptoSchemeSupported(const uint8_t uuid[16]) const = 0;

    virtual status_t createPlugin(
            const uint8_t uuid[16], const void *data, size_t size,
            CryptoPlugin **plugin) = 0;

private:
    CryptoFactory(const CryptoFactory &);
    CryptoFactory &operator=(const CryptoFactory &);
};

struct CryptoPlugin {
    enum Mode {
        kMode_Unencrypted = 0,
        kMode_AES_CTR     = 1,

        // Neither key nor iv are being used in this mode.
        // Each subsample is encrypted w/ an iv of all zeroes.
        kMode_AES_WV      = 2,  // FIX constant
    };

    struct SubSample {
        size_t mNumBytesOfClearData;
        size_t mNumBytesOfEncryptedData;
    };

    CryptoPlugin() {}
    virtual ~CryptoPlugin() {}

    // If this method returns false, a non-secure decoder will be used to
    // decode the data after decryption. The decrypt API below will have
    // to support insecure decryption of the data (secure = false) for
    // media data of the given mime type.
    virtual bool requiresSecureDecoderComponent(const char *mime) const = 0;

    // If the error returned falls into the range
    // ERROR_DRM_VENDOR_MIN..ERROR_DRM_VENDOR_MAX, errorDetailMsg should be
    // filled in with an appropriate string.
    // At the java level these special errors will then trigger a
    // MediaCodec.CryptoException that gives clients access to both
    // the error code and the errorDetailMsg.
#ifdef PLATFORM_VERSION_V4_1_2
    // Returns a non-negative result to indicate the number of bytes written
    // to the dstPtr, or a negative result to indicate an error.
    virtual ssize_t decrypt(
#else
    virtual status_t decrypt(
#endif
            bool secure,
            const uint8_t key[16],
            const uint8_t iv[16],
            Mode mode,
            const void *srcPtr,
            const SubSample *subSamples, size_t numSubSamples,
            void *dstPtr,
            AString *errorDetailMsg) = 0;

private:
    CryptoPlugin(const CryptoPlugin &);
    CryptoPlugin &operator=(const CryptoPlugin &);
};

}  // namespace android

extern "C" {
    extern android::CryptoFactory *createCryptoFactory();
}

#endif  // CRYPTO_API_H_
