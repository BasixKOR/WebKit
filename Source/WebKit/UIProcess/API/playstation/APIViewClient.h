/*
 * Copyright (C) 2021 Sony Interactive Entertainment Inc.
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

#include <wtf/CompletionHandler.h>
#include <wtf/TZoneMallocInlines.h>

namespace WebCore {
class Cursor;
class IntRect;
class Region;
}

namespace WebKit {
class PlayStationWebView;
}

namespace API {

class ViewClient {
    WTF_MAKE_TZONE_ALLOCATED_INLINE(ViewClient);
public:
    virtual ~ViewClient() = default;

    virtual void setViewNeedsDisplay(WebKit::PlayStationWebView&, const WebCore::Region&) { }
    virtual void enterFullScreen(WebKit::PlayStationWebView&, CompletionHandler<void(bool)>&& completionHandler) { completionHandler(false); }
    virtual void exitFullScreen(WebKit::PlayStationWebView&) { }
    virtual void closeFullScreen(WebKit::PlayStationWebView&) { }
    virtual void beganEnterFullScreen(WebKit::PlayStationWebView&, const WebCore::IntRect&, const WebCore::IntRect&, CompletionHandler<void(bool)>&& completionHandler) { completionHandler(false); }
    virtual void beganExitFullScreen(WebKit::PlayStationWebView&, const WebCore::IntRect&, const WebCore::IntRect&, CompletionHandler<void()>&& completionHandler) { completionHandler(); }
    virtual void setCursor(WebKit::PlayStationWebView& view, const WebCore::Cursor& cursor) { }
};

} // namespace API
