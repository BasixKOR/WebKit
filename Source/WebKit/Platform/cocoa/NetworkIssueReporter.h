/*
 * Copyright (C) 2022 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#if ENABLE(NETWORK_ISSUE_REPORTING)

#import <wtf/Forward.h>
#import <wtf/TZoneMalloc.h>

OBJC_CLASS NSURLSessionTaskMetrics;

namespace WebKit {

class NetworkIssueReporter {
    WTF_MAKE_TZONE_ALLOCATED(NetworkIssueReporter);
    WTF_MAKE_NONCOPYABLE(NetworkIssueReporter);
public:
    NetworkIssueReporter();
    ~NetworkIssueReporter();

    void report(const URL&);

    static bool isEnabled();
    static bool shouldReport(NSURLSessionTaskMetrics *);

private:
    HashSet<String> m_reportedHosts;
    void* m_stackTrace { nullptr };
    size_t m_stackTraceSize { 0 };
};

} // namespace WebKit

#endif // ENABLE(NETWORK_ISSUE_REPORTING)
