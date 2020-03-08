/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "SkAutoPixmapStorage.h"
#include "SkData.h"

SkAutoPixmapStorage::SkAutoPixmapStorage() : fStorage(nullptr) {}

SkAutoPixmapStorage::~SkAutoPixmapStorage() {
    this->freeStorage();
}

SkAutoPixmapStorage& SkAutoPixmapStorage::operator=(SkAutoPixmapStorage&& other) {
    this->fStorage = other.fStorage;
    this->INHERITED::reset(other.info(), this->fStorage, other.rowBytes());

    other.fStorage = nullptr;
    other.INHERITED::reset();

    return *this;
}

size_t SkAutoPixmapStorage::AllocSize(const SkImageInfo& info, size_t* rowBytes) {
    size_t rb = info.minRowBytes();
    if (rowBytes) {
        *rowBytes = rb;
    }
    return info.computeByteSize(rb);
}

bool SkAutoPixmapStorage::tryAlloc(const SkImageInfo& info) {
    this->freeStorage();

    size_t rb;
    size_t size = AllocSize(info, &rb);
    if (SkImageInfo::ByteSizeOverflowed(size)) {
        return false;
    }
    void* pixels = sk_malloc_flags(size, 0);
    if (nullptr == pixels) {
        return false;
    }
    this->reset(info, pixels, rb);
    fStorage = pixels;
    return true;
}

void SkAutoPixmapStorage::alloc(const SkImageInfo& info) {
    SkASSERT_RELEASE(this->tryAlloc(info));
}

const SkData* SkAutoPixmapStorage::detachPixelsAsData() {
    if (!fStorage) {
        return nullptr;
    }

    auto data = SkData::MakeFromMalloc(fStorage, this->computeByteSize());
    fStorage = nullptr;
    this->INHERITED::reset();

    return data.release();
}
