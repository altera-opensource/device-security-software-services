/*
 * This project is licensed as below.
 *
 * **************************************************************************
 *
 * Copyright 2020-2025 Altera Corporation. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * **************************************************************************
 */


#include "quartus/protected_memory_test.h"

/**
 * ProtectedMemory tests
 */

void assertPageGuarded(const ProtectedMemory::Page &page) {
    ASSERT_EQ(ProtectedMemory::PageState::PAGE_GUARDED, page.state());
}

void assertTooManyPages(const ProtectedMemory::Page &page) {
    ASSERT_EQ(ProtectedMemory::PageState::TOO_MANY_PAGES, page.state());
}

void assertNumAllocatedPages(const ProtectedMemory::Page &page, uint8_t expectedAllocatedPages) {
    ASSERT_EQ(expectedAllocatedPages, page.num_pages());
}

TEST_F(ProtectedMemoryTest, allocate0Bytes_Returns0Pages)
{
    // given
    size_t requestedMemorySize = 0;
    uint8_t expectedAllocatedPages = 0;

    // when
    ProtectedMemory::Page page(requestedMemorySize);

    // then
    assertPageGuarded(page);
    assertNumAllocatedPages(page, expectedAllocatedPages);
}

TEST_F(ProtectedMemoryTest, allocate1Byte_Returns1Page)
{
    // given
    size_t requestedMemorySize = 1;
    uint8_t expectedAllocatedPages = 1;

    // when
    ProtectedMemory::Page page(requestedMemorySize);

    // then
    assertPageGuarded(page);
    assertNumAllocatedPages(page, expectedAllocatedPages);
}

TEST_F(ProtectedMemoryTest, allocate2Bytes_Returns1Page)
{
    // given
    size_t requestedMemorySize = 2;
    uint8_t expectedAllocatedPages = 1;

    // when
    ProtectedMemory::Page page(requestedMemorySize);

    // then
    assertPageGuarded(page);
    assertNumAllocatedPages(page, expectedAllocatedPages);
}

TEST_F(ProtectedMemoryTest, allocate1PageSize_Returns1Page)
{
    // given
    size_t requestedMemorySize = PAGE_SIZE;
    uint8_t expectedAllocatedPages = 1;

    // when
    ProtectedMemory::Page page(requestedMemorySize);

    // then
    assertPageGuarded(page);
    assertNumAllocatedPages(page, expectedAllocatedPages);
}

TEST_F(ProtectedMemoryTest, allocate1PageSizePlusOneByte_Returns2Pages)
{
    // given
    size_t requestedMemorySize = PAGE_SIZE+1;
    uint8_t expectedAllocatedPages = 2;

    // when
    ProtectedMemory::Page page(requestedMemorySize);

    // then
    assertPageGuarded(page);
    assertNumAllocatedPages(page, expectedAllocatedPages);
}

TEST_F(ProtectedMemoryTest, allocate10PagesSize_Returns10Pages)
{
    // given
    size_t requestedMemorySize = PAGE_SIZE*10;
    uint8_t expectedAllocatedPages = 10;

    // when
    ProtectedMemory::Page page(requestedMemorySize);

    // then
    assertPageGuarded(page);
    assertNumAllocatedPages(page, expectedAllocatedPages);
}


TEST_F(ProtectedMemoryTest, allocateMaxPagesSizePlusOneByte_ReturnsTooManyPages)
{
    // given
    size_t requestedMemorySize = PAGE_SIZE*253 + 1;
    uint8_t expectedAllocatedPages = 0;

    // when
    ProtectedMemory::Page page(requestedMemorySize);

    // then
    assertTooManyPages(page);
    assertNumAllocatedPages(page, expectedAllocatedPages);
}

TEST_F(ProtectedMemoryTest, allocateVeryBigValue_ReturnsTooManyPages)
{
    // given
    size_t requestedMemorySize = -1;
    uint8_t expectedAllocatedPages = 0;

    // when
    ProtectedMemory::Page page(requestedMemorySize);

    // then
    assertTooManyPages(page);
    assertNumAllocatedPages(page, expectedAllocatedPages);
}
