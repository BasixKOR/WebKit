/*
 * Copyright 2010, The Android Open Source Project
 * Copyright (C) 2012 Samsung Electronics. All rights reserved.
 * Copyright (C) 2024 Apple Inc. All rights reserved.
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

#include "config.h"
#include "DeviceOrientationController.h"

#include "DeviceOrientationClient.h"
#include "DeviceOrientationData.h"
#include "DeviceOrientationEvent.h"
#include "EventNames.h"
#include "Page.h"
#include <wtf/TZoneMallocInlines.h>

namespace WebCore {

WTF_MAKE_TZONE_ALLOCATED_IMPL(DeviceOrientationClient);
WTF_MAKE_TZONE_ALLOCATED_IMPL(DeviceOrientationController);

DeviceOrientationController::DeviceOrientationController(DeviceOrientationClient& client)
    : m_client(client)
{
    client.setController(this);
}

void DeviceOrientationController::didChangeDeviceOrientation(DeviceOrientationData* orientation)
{
    dispatchDeviceEvent(DeviceOrientationEvent::create(eventNames().deviceorientationEvent, orientation));
}

#if PLATFORM(IOS_FAMILY)

// FIXME: We should look to reconcile the iOS and non-iOS differences with this class
// so that we can either remove these functions or remove the PLATFORM(IOS_FAMILY)-guard.

void DeviceOrientationController::suspendUpdates()
{
    m_client->stopUpdating();
}

void DeviceOrientationController::resumeUpdates(const SecurityOriginData& origin)
{
    if (hasListeners())
        m_client->startUpdating(origin);
}

#else

bool DeviceOrientationController::hasLastData()
{
    return m_client->lastOrientation();
}

RefPtr<Event> DeviceOrientationController::getLastEvent()
{
    RefPtr orientation = m_client->lastOrientation();
    return DeviceOrientationEvent::create(eventNames().deviceorientationEvent, orientation.get());
}

#endif // PLATFORM(IOS_FAMILY)

ASCIILiteral DeviceOrientationController::supplementName()
{
    return "DeviceOrientationController"_s;
}

DeviceOrientationController* DeviceOrientationController::from(Page* page)
{
    return static_cast<DeviceOrientationController*>(Supplement<Page>::from(page, supplementName()));
}

bool DeviceOrientationController::isActiveAt(Page* page)
{
    if (DeviceOrientationController* self = DeviceOrientationController::from(page))
        return self->isActive();
    return false;
}

DeviceClient& DeviceOrientationController::client()
{
    return m_client.get();
}

} // namespace WebCore
