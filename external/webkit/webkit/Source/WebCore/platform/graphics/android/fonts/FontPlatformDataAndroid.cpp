/*
 * Copyright 2009, The Android Open Source Project
 * Copyright (C) 2006 Apple Computer, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//This file is part of the internal font implementation.  It should not be included by anyone other than
// FontMac.cpp, FontWin.cpp and Font.cpp.

#include "config.h"
#include "FontPlatformData.h"

#ifdef SUPPORT_COMPLEX_SCRIPTS
#include "HarfbuzzSkia.h"
#endif
#include "SkAdvancedTypefaceMetrics.h"
#include "SkPaint.h"
#include "SkTypeface.h"

//#define TRACE_FONTPLATFORMDATA_LIFE
//#define COUNT_FONTPLATFORMDATA_LIFE

#ifdef COUNT_FONTPLATFORMDATA_LIFE
static int gCount;
static int gMaxCount;

static void inc_count()
{
    if (++gCount > gMaxCount)
    {
        gMaxCount = gCount;
        SkDebugf("---------- FontPlatformData %d\n", gMaxCount);
    }
}

static void dec_count() { --gCount; }
#else
    #define inc_count()
    #define dec_count()
#endif

#ifdef TRACE_FONTPLATFORMDATA_LIFE
    #define trace(num)  SkDebugf("FontPlatformData%d %p %g %d %d\n", num, mTypeface, mTextSize, mFakeBold, mFakeItalic)
#else
    #define trace(num)
#endif

namespace WebCore {

FontPlatformData::RefCountedHarfbuzzFace::~RefCountedHarfbuzzFace()
{
#ifdef SUPPORT_COMPLEX_SCRIPTS
    HB_FreeFace(m_harfbuzzFace);
#endif
}

FontPlatformData::FontPlatformData()
    : mTypeface(NULL), mTextSize(0), mEmSizeInFontUnits(0), mFakeBold(false), mFakeItalic(false),
#ifdef FONT_SOFTWARE_RENDER
      mForceFakeBold(0), mForceFakeItalic(0),
#endif
      mOrientation(Horizontal), mTextOrientation(TextOrientationVerticalRight)
{
    inc_count();
    trace(1);
}

FontPlatformData::FontPlatformData(const FontPlatformData& src)
{
    if (hashTableDeletedFontValue() != src.mTypeface) {
        SkSafeRef(src.mTypeface);
    }

    mTypeface = src.mTypeface;
    mTextSize = src.mTextSize;
    mEmSizeInFontUnits = src.mEmSizeInFontUnits;
    mFakeBold = src.mFakeBold;
    mFakeItalic = src.mFakeItalic;
    m_harfbuzzFace = src.m_harfbuzzFace;
    mOrientation = src.mOrientation;
    mTextOrientation = src.mTextOrientation;
#ifdef FONT_SOFTWARE_RENDER
    mForceFakeBold = src.mForceFakeBold;
    mForceFakeItalic = src.mForceFakeItalic;
#endif

    inc_count();
    trace(2);
}

FontPlatformData::FontPlatformData(SkTypeface* tf, float textSize, bool fakeBold, bool fakeItalic,
    FontOrientation orientation, TextOrientation textOrientation)
    : mTypeface(tf), mTextSize(textSize), mEmSizeInFontUnits(0), mFakeBold(fakeBold), mFakeItalic(fakeItalic),
#ifdef FONT_SOFTWARE_RENDER
      mForceFakeBold(0), mForceFakeItalic(0),
#endif
      mOrientation(orientation), mTextOrientation(textOrientation)
{
    if (hashTableDeletedFontValue() != mTypeface) {
        SkSafeRef(mTypeface);
    }

    inc_count();
    trace(3);
}

FontPlatformData::FontPlatformData(const FontPlatformData& src, float textSize)
    : mTypeface(src.mTypeface), mTextSize(textSize), mEmSizeInFontUnits(src.mEmSizeInFontUnits), mFakeBold(src.mFakeBold), mFakeItalic(src.mFakeItalic),
#ifdef FONT_SOFTWARE_RENDER
      mForceFakeBold(src.mForceFakeBold), mForceFakeItalic(src.mForceFakeItalic),
#endif
      mOrientation(src.mOrientation), mTextOrientation(src.mTextOrientation), m_harfbuzzFace(src.m_harfbuzzFace)
{
    if (hashTableDeletedFontValue() != mTypeface) {
        SkSafeRef(mTypeface);
    }

    inc_count();
    trace(4);
}

FontPlatformData::FontPlatformData(float size, bool bold, bool oblique)
    : mTypeface(NULL), mTextSize(size),  mEmSizeInFontUnits(0), mFakeBold(bold), mFakeItalic(oblique),
#ifdef FONT_SOFTWARE_RENDER
      mForceFakeBold(0), mForceFakeItalic(0),
#endif
      mOrientation(Horizontal), mTextOrientation(TextOrientationVerticalRight)
{
    inc_count();
    trace(5);
}

FontPlatformData::FontPlatformData(const FontPlatformData& src, SkTypeface* tf)
    : mTypeface(tf), mTextSize(src.mTextSize),  mEmSizeInFontUnits(0), mFakeBold(src.mFakeBold),
      mFakeItalic(src.mFakeItalic), mOrientation(src.mOrientation),
#ifdef FONT_SOFTWARE_RENDER
      mForceFakeBold(src.mForceFakeBold), mForceFakeItalic(src.mForceFakeItalic),
#endif
      mTextOrientation(src.mTextOrientation)
{
    if (hashTableDeletedFontValue() != mTypeface) {
        SkSafeRef(mTypeface);
    }

    inc_count();
    trace(6);
}

FontPlatformData::~FontPlatformData()
{
    dec_count();
#ifdef TRACE_FONTPLATFORMDATA_LIFE
    SkDebugf("----------- ~FontPlatformData\n");
#endif

    if (hashTableDeletedFontValue() != mTypeface) {
        SkSafeUnref(mTypeface);
    }
}

int FontPlatformData::emSizeInFontUnits() const
{
    if (mEmSizeInFontUnits)
        return mEmSizeInFontUnits;

    SkAdvancedTypefaceMetrics* metrics = 0;
    if (mTypeface)
        metrics = mTypeface->getAdvancedTypefaceMetrics(SkAdvancedTypefaceMetrics::kNo_PerGlyphInfo);
    if (metrics) {
        mEmSizeInFontUnits = metrics->fEmSize;
        metrics->unref();
    } else
        mEmSizeInFontUnits = 1000;  // default value copied from Skia.
    return mEmSizeInFontUnits;
}

FontPlatformData& FontPlatformData::operator=(const FontPlatformData& src)
{
    if (hashTableDeletedFontValue() != src.mTypeface) {
        SkSafeRef(src.mTypeface);
    }
    if (hashTableDeletedFontValue() != mTypeface) {
        SkSafeUnref(mTypeface);
    }

    mTypeface = src.mTypeface;
    mEmSizeInFontUnits = src.mEmSizeInFontUnits;
    mTextSize = src.mTextSize;
    mFakeBold = src.mFakeBold;
    mFakeItalic = src.mFakeItalic;
    m_harfbuzzFace = src.m_harfbuzzFace;
    mOrientation = src.mOrientation;
    mTextOrientation = src.mTextOrientation;
#ifdef FONT_SOFTWARE_RENDER
    mForceFakeBold = src.mForceFakeBold;
    mForceFakeItalic = src.mForceFakeItalic;
#endif

    return *this;
}

void FontPlatformData::setupPaint(SkPaint* paint) const
{
    if (hashTableDeletedFontValue() == mTypeface)
        paint->setTypeface(0);
    else
        paint->setTypeface(mTypeface);

    paint->setAntiAlias(true);
    paint->setSubpixelText(true);
    paint->setHinting(SkPaint::kSlight_Hinting);
    paint->setTextSize(SkFloatToScalar(mTextSize));
#ifdef FONT_SOFTWARE_RENDER
    if (!mForceFakeBold & 0x02)
        paint->setFakeBoldText(mFakeBold);
    else
        paint->setFakeBoldText(mForceFakeBold & 0x01);
    if (!mForceFakeItalic & 0x02)
        paint->setTextSkewX(mFakeItalic ? -SK_Scalar1/4 : 0);
    else
        paint->setTextSkewX((mForceFakeItalic & 0x01) ? -SK_Scalar1/4 : 0);
#else
    paint->setFakeBoldText(mFakeBold);
    paint->setTextSkewX(mFakeItalic ? -SK_Scalar1/4 : 0);
#endif
#ifndef SUPPORT_COMPLEX_SCRIPTS
    paint->setTextEncoding(SkPaint::kUTF16_TextEncoding);
#endif
}

uint32_t FontPlatformData::uniqueID() const
{
    if (hashTableDeletedFontValue() == mTypeface)
        return SkTypeface::UniqueID(0);
    else
        return SkTypeface::UniqueID(mTypeface);
}

bool FontPlatformData::operator==(const FontPlatformData& a) const
{
    return  mTypeface == a.mTypeface &&
            mTextSize == a.mTextSize &&
            mFakeBold == a.mFakeBold &&
            mFakeItalic == a.mFakeItalic &&
#ifdef FONT_SOFTWARE_RENDER
            mForceFakeBold == a.mForceFakeBold &&
            mForceFakeItalic == a.mForceFakeItalic &&
#endif
            mOrientation == a.mOrientation &&
            mTextOrientation == a.mTextOrientation;
}

unsigned FontPlatformData::hash() const
{
    unsigned h;

    if (hashTableDeletedFontValue() == mTypeface) {
        h = reinterpret_cast<unsigned>(mTypeface);
    } else {
        h = SkTypeface::UniqueID(mTypeface);
    }

    uint32_t sizeAsInt = *reinterpret_cast<const uint32_t*>(&mTextSize);

#ifdef FONT_SOFTWARE_RENDER
    h ^= 0x01010101 * (((int)mForceFakeBold << 5) | ((int)mForceFakeItalic << 4) |
         (static_cast<int>(mTextOrientation) << 3) | (static_cast<int>(mOrientation) << 2) |
         ((int)mFakeBold << 1) | (int)mFakeItalic);
#else
    h ^= 0x01010101 * ((static_cast<int>(mTextOrientation) << 3) | (static_cast<int>(mOrientation) << 2) |
         ((int)mFakeBold << 1) | (int)mFakeItalic);
#endif
    h ^= sizeAsInt;
    return h;
}

bool FontPlatformData::isFixedPitch() const
{
    if (mTypeface && (mTypeface != hashTableDeletedFontValue()))
        return mTypeface->isFixedWidth();
    else
        return false;
}

#ifdef FONT_SOFTWARE_RENDER
void FontPlatformData::setFakeAttr(bool fakeBold, bool fakeItalic)
{
    mForceFakeBold = 0;
    mForceFakeItalic = 0;
    mForceFakeBold |= (0x02 | (fakeBold ? 0x01 : 0));
    mForceFakeItalic |= (0x02 | (fakeItalic ? 0x01 : 0));
}
#endif

HB_FaceRec_* FontPlatformData::harfbuzzFace() const
{
#ifdef SUPPORT_COMPLEX_SCRIPTS
    if (!m_harfbuzzFace)
        m_harfbuzzFace = RefCountedHarfbuzzFace::create(
            HB_NewFace(const_cast<FontPlatformData*>(this), harfbuzzSkiaGetTable));

    return m_harfbuzzFace->face();
#else
    return NULL;
#endif
}
}
