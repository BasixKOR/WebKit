/*
 * Copyright (C) 2019 Apple Inc. All rights reserved.
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

#include "config.h"
#include "RemoteMediaResourceLoader.h"

#if ENABLE(GPU_PROCESS) && ENABLE(VIDEO)

#include "RemoteMediaPlayerProxy.h"
#include <WebCore/ResourceError.h>
#include <wtf/TZoneMallocInlines.h>

namespace WebKit {

WTF_MAKE_TZONE_ALLOCATED_IMPL(RemoteMediaResourceLoader);

using namespace WebCore;

RemoteMediaResourceLoader::RemoteMediaResourceLoader(RemoteMediaPlayerProxy& remoteMediaPlayerProxy)
    : m_remoteMediaPlayerProxy(remoteMediaPlayerProxy)
{
    ASSERT(isMainRunLoop());
}

RemoteMediaResourceLoader::~RemoteMediaResourceLoader()
{
}

RefPtr<PlatformMediaResource> RemoteMediaResourceLoader::requestResource(ResourceRequest&& request, LoadOptions options)
{
    ASSERT(isMainRunLoop());
    RefPtr remoteMediaPlayerProxy = m_remoteMediaPlayerProxy.get();
    if (!remoteMediaPlayerProxy)
        return nullptr;

    return remoteMediaPlayerProxy->requestResource(WTFMove(request), options);
}

void RemoteMediaResourceLoader::sendH2Ping(const URL& url, CompletionHandler<void(Expected<Seconds, ResourceError>&&)>&& completionHandler)
{
    ASSERT(isMainRunLoop());
    RefPtr remoteMediaPlayerProxy = m_remoteMediaPlayerProxy.get();
    if (!remoteMediaPlayerProxy)
        return completionHandler(makeUnexpected(internalError(url)));
    
    remoteMediaPlayerProxy->sendH2Ping(url, WTFMove(completionHandler));
}

} // namespace WebKit

#endif // ENABLE(GPU_PROCESS) && ENABLE(VIDEO)
