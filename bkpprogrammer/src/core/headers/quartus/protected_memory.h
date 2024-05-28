/*
 * This project is licensed as below.
 *
 * **************************************************************************
 *
 * Copyright 2020-2024 Intel Corporation. All Rights Reserved.
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

#ifndef PROGRAMMER_PROTECTED_MEMORY_H
#define PROGRAMMER_PROTECTED_MEMORY_H

#include <sys/mman.h>
#include <unistd.h>
#include <iostream>
#include <climits>
#include <cstring>

namespace ProtectedMemory {

    enum class PageState {
        PAGE_GUARDED,
        TOO_MANY_PAGES,
        PAGE_NOT_ALLOCATED,
        PAGE_NOT_PROTECTED,
        PAGE_NOT_LOCKED,
    };

/**
 *
 * Class responsible for memory allocation. Requested PAGE Number allocation (central page)
 * is surrounded by two guard pages. Permissions of central page is set to PROT_READ | PROT_WRITE,
 * while guard pages are left without permissions (PROT_NONE).
 *
 * Due to two guard pages, memory allocation can't be overwritten to or from.
 *
 * Maximum allowed contiguously allocated pages number is: UCHAR_MAX - 2.
 *
 * 0x00007ffff7ff4000 0x00007ffff7ff5000 ---p	mapped <- guard page
 * 0x00007ffff7ff5000 0x00007ffff7ff6000 rw-p	mapped <- central page
 * 0x00007ffff7ff6000 0x00007ffff7ff7000 ---p	mapped <- guard page
 *
 */

    class Page {

        const uint8_t m_page_guards = 2;
        const uint8_t m_max_pages = UCHAR_MAX - m_page_guards;

        bool m_lock_page; //do not swap
        uint8_t* m_raw_address = nullptr;
        size_t m_memory_size = 0;
        long m_page_size = 0L;
        uint8_t m_pages_num = 0;
        PageState m_state;

        static long get_page_size() noexcept {
            return sysconf(_SC_PAGE_SIZE);
        }

        static long determine_pages_num(size_t requested_mem_size, long page_size) noexcept {
            if (requested_mem_size == 0) {
                return 0;
            }

            size_t ratio = requested_mem_size / page_size;
            return (requested_mem_size % page_size == 0) ? ratio : ratio + 1;
        }

        bool pageCanBeUnlocked() noexcept {
            return (m_lock_page && PageState::PAGE_GUARDED == m_state);
        }

        bool pageCanBeUnmapped() noexcept {
            return (PageState::PAGE_NOT_ALLOCATED != m_state && PageState::TOO_MANY_PAGES != m_state);
        }

    public:
        explicit Page(size_t requested_mem_size_bytes, bool lock = true) noexcept {
            m_state = PageState::PAGE_NOT_ALLOCATED;
            m_lock_page = lock;

            m_page_size = get_page_size();

            if(m_page_size == -1)
                return;

            long pages_num = determine_pages_num(requested_mem_size_bytes, m_page_size);

            /**
             * verify uint8 overflow
             */
            if (pages_num > m_max_pages) {
                m_state = PageState::TOO_MANY_PAGES;
                return;
            }

            m_pages_num = pages_num;

            m_memory_size = (m_pages_num + m_page_guards) * m_page_size;

            /**
             * allocate requested pages plus two additional guard pages
             */
            m_raw_address = static_cast<uint8_t*>(mmap(nullptr, m_memory_size,
                                                       PROT_NONE,
                                                       MAP_PRIVATE | MAP_ANONYMOUS,
                                                       0, 0));

            if(!m_raw_address) {
                m_state = PageState::PAGE_NOT_ALLOCATED;
                return;
            }

            /**
             * Perform extensive TLB unvalidation by modifying central page permissions
             */
            if(mprotect(m_raw_address + m_page_size, m_page_size * m_pages_num, PROT_READ | PROT_WRITE ) != 0) {
                m_state = PageState::PAGE_NOT_PROTECTED;
                return;
            }

            /**
             * Lock central page so it's not swpaped to disk
             */
            if(m_lock_page) {
                if(mlock(m_raw_address + m_page_size, m_pages_num * m_page_size) != 0) {
                    m_state = PageState::PAGE_NOT_LOCKED;
                    return;
                }
            }

            m_state = PageState::PAGE_GUARDED;
        }

        PageState state() const {
            return m_state;
        }

        uint8_t num_pages() const {
            return m_pages_num;
        }

        /**
         * return address of protected allocation (read+write)
         */
        void* get() {
            return static_cast<void*>(m_raw_address + m_page_size);
        }

        virtual ~Page() noexcept {
            if (pageCanBeUnlocked())
                munlock(static_cast<uint8_t*>(m_raw_address) + m_page_size, m_pages_num * m_page_size);

            if (pageCanBeUnmapped())
                munmap(m_raw_address, m_memory_size);
        }

        Page(const Page&) = delete;
        Page& operator=(const Page&) = delete;
    };
};

#endif //PROGRAMMER_PROTECTED_MEMORY_H
