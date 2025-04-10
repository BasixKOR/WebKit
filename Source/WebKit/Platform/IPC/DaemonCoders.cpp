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

#include "config.h"
#include "DaemonCoders.h"

#include "DaemonDecoder.h"
#include "DaemonEncoder.h"
#include <WebCore/CertificateInfo.h>
#include <WebCore/PrivateClickMeasurement.h>
#include <WebCore/PushSubscriptionData.h>
#include <WebCore/RegistrableDomain.h>

#if PLATFORM(COCOA)
#include <CoreFoundation/CoreFoundation.h>
#include <wtf/cf/VectorCF.h>
#include <wtf/spi/cocoa/SecuritySPI.h>
#endif

namespace WebKit::Daemon {

void Coder<WTF::WallTime>::encode(Encoder& encoder, const WTF::WallTime& instance)
{
    encoder << instance.secondsSinceEpoch().value();
}

std::optional<WTF::WallTime> Coder<WTF::WallTime>::decode(Decoder& decoder)
{
    std::optional<double> value;
    decoder >> value;
    if (!value)
        return std::nullopt;
    return WallTime::fromRawSeconds(*value);
}

void Coder<WebCore::CertificateInfo>::encode(Encoder& encoder, const WebCore::CertificateInfo& instance)
{
#if PLATFORM(COCOA)
    RetainPtr data = adoptCF(SecTrustSerialize(instance.trust().get(), nullptr));
    if (!data) {
        encoder << false;
        return;
    }
    encoder << true;
    uint64_t length = CFDataGetLength(data.get());
    encoder << length;
    encoder.encodeFixedLengthData(span(data.get()));
#endif
}

std::optional<WebCore::CertificateInfo> Coder<WebCore::CertificateInfo>::decode(Decoder& decoder)
{
#if PLATFORM(COCOA)
    std::optional<bool> hasTrust;
    decoder >> hasTrust;
    if (!hasTrust)
        return std::nullopt;
    if (!*hasTrust)
        return WebCore::CertificateInfo();
    std::optional<uint64_t> length;
    decoder >> length;
    if (!length)
        return std::nullopt;
    std::span<const uint8_t> bytes = decoder.decodeFixedLengthReference(*length);
    if (bytes.size() != *length)
        return std::nullopt;
    auto trust = adoptCF(SecTrustDeserialize(adoptCF(CFDataCreate(nullptr, bytes.data(), bytes.size())).get(), nullptr));
    if (!trust)
        return std::nullopt;
    return WebCore::CertificateInfo(WTFMove(trust));
#else
    return WebCore::CertificateInfo();
#endif
}

template<> struct Coder<WebCore::PCM::SourceSite> {
    static void encode(Encoder& encoder, const WebCore::PCM::SourceSite& instance)
    {
        encoder << instance.registrableDomain;
    }
    static std::optional<WebCore::PCM::SourceSite> decode(Decoder& decoder)
    {
        std::optional<WebCore::RegistrableDomain> registrableDomain;
        decoder >> registrableDomain;
        if (!registrableDomain)
            return std::nullopt;
        return { WebCore::PCM::SourceSite { WTFMove(*registrableDomain) } };
    }
};

template<> struct Coder<WebCore::PCM::AttributionDestinationSite> {
    static void encode(Encoder& encoder, const WebCore::PCM::AttributionDestinationSite& instance)
    {
        encoder << instance.registrableDomain;
    }
    static std::optional<WebCore::PCM::AttributionDestinationSite> decode(Decoder& decoder)
    {
        std::optional<WebCore::RegistrableDomain> registrableDomain;
        decoder >> registrableDomain;
        if (!registrableDomain)
            return std::nullopt;
        return { WebCore::PCM::AttributionDestinationSite { WTFMove(*registrableDomain) } };
    }
};

template<> struct Coder<WebCore::PCM::EphemeralNonce> {
    static void encode(Encoder& encoder, const WebCore::PCM::EphemeralNonce& instance)
    {
        encoder << instance.nonce;
    }
    static std::optional<WebCore::PCM::EphemeralNonce> decode(Decoder& decoder)
    {
        std::optional<String> nonce;
        decoder >> nonce;
        if (!nonce)
            return std::nullopt;
        return { WebCore::PCM::EphemeralNonce { WTFMove(*nonce) } };
    }
};

template<> struct Coder<WebCore::PCM::AttributionTimeToSendData> {
    static void encode(Encoder& encoder, const WebCore::PCM::AttributionTimeToSendData& instance)
    {
        encoder << instance.sourceEarliestTimeToSend;
        encoder << instance.destinationEarliestTimeToSend;
    }
    static std::optional<WebCore::PCM::AttributionTimeToSendData> decode(Decoder& decoder)
    {
        std::optional<std::optional<WallTime>> sourceEarliestTimeToSend;
        decoder >> sourceEarliestTimeToSend;
        if (!sourceEarliestTimeToSend)
            return std::nullopt;

        std::optional<std::optional<WallTime>> destinationEarliestTimeToSend;
        decoder >> destinationEarliestTimeToSend;
        if (!destinationEarliestTimeToSend)
            return std::nullopt;

        return { { WTFMove(*sourceEarliestTimeToSend), WTFMove(*destinationEarliestTimeToSend) } };
    }
};

void Coder<WebCore::PrivateClickMeasurement, void>::encode(Encoder& encoder, const WebCore::PrivateClickMeasurement& instance)
{
    encoder << instance.sourceID();
    encoder << instance.sourceSite();
    encoder << instance.destinationSite();
    encoder << instance.timeOfAdClick();
    encoder << instance.isEphemeral();
    encoder << instance.adamID();
    encoder << instance.attributionTriggerData();
    encoder << instance.timesToSend();
    encoder << instance.ephemeralSourceNonce();
    encoder << instance.sourceApplicationBundleID();
}

std::optional<WebCore::PrivateClickMeasurement> Coder<WebCore::PrivateClickMeasurement, void>::decode(Decoder& decoder)
{
    std::optional<uint8_t> sourceID;
    decoder >> sourceID;
    if (!sourceID)
        return std::nullopt;

    std::optional<WebCore::PCM::SourceSite> sourceSite;
    decoder >> sourceSite;
    if (!sourceSite)
        return std::nullopt;

    std::optional<WebCore::PCM::AttributionDestinationSite> destinationSite;
    decoder >> destinationSite;
    if (!destinationSite)
        return std::nullopt;

    std::optional<WallTime> timeOfAdClick;
    decoder >> timeOfAdClick;
    if (!timeOfAdClick)
        return std::nullopt;

    std::optional<WebCore::PCM::AttributionEphemeral> isEphemeral;
    decoder >> isEphemeral;
    if (!isEphemeral)
        return std::nullopt;

    std::optional<std::optional<uint64_t>> adamID;
    decoder >> adamID;
    if (!adamID)
        return std::nullopt;

    std::optional<std::optional<WebCore::PCM::AttributionTriggerData>> attributionTriggerData;
    decoder >> attributionTriggerData;
    if (!attributionTriggerData)
        return std::nullopt;

    std::optional<WebCore::PCM::AttributionTimeToSendData> timesToSend;
    decoder >> timesToSend;
    if (!timesToSend)
        return std::nullopt;

    std::optional<std::optional<WebCore::PCM::EphemeralNonce>> ephemeralSourceNonce;
    decoder >> ephemeralSourceNonce;
    if (!ephemeralSourceNonce)
        return std::nullopt;

    std::optional<String> sourceApplicationBundleID;
    decoder >> sourceApplicationBundleID;
    if (!sourceApplicationBundleID)
        return std::nullopt;

    return { {
        WTFMove(*sourceID),
        WTFMove(*sourceSite),
        WTFMove(*destinationSite),
        WTFMove(*timeOfAdClick),
        WTFMove(*isEphemeral),
        WTFMove(*adamID),
        WTFMove(*attributionTriggerData),
        WTFMove(*timesToSend),
        WTFMove(*ephemeralSourceNonce),
        WTFMove(*sourceApplicationBundleID),
    } };
}

void Coder<WebCore::PCM::AttributionTriggerData, void>::encode(Encoder& encoder, const WebCore::PCM::AttributionTriggerData& instance)
{
    encoder << instance.data;
    encoder << instance.priority;
    encoder << instance.wasSent;
    encoder << instance.sourceRegistrableDomain;
    encoder << instance.ephemeralDestinationNonce;
    encoder << instance.destinationSite;
}

std::optional<WebCore::PCM::AttributionTriggerData> Coder<WebCore::PCM::AttributionTriggerData, void>::decode(Decoder& decoder)
{
    std::optional<uint8_t> data;
    decoder >> data;
    if (!data)
        return std::nullopt;

    std::optional<WebCore::PCM::AttributionTriggerData::Priority::PriorityValue> priority;
    decoder >> priority;
    if (!priority)
        return std::nullopt;

    std::optional<WebCore::PCM::WasSent> wasSent;
    decoder >> wasSent;
    if (!wasSent)
        return std::nullopt;

    std::optional<std::optional<WebCore::RegistrableDomain>> sourceRegistrableDomain;
    decoder >> sourceRegistrableDomain;
    if (!sourceRegistrableDomain)
        return std::nullopt;

    std::optional<std::optional<WebCore::PCM::EphemeralNonce>> ephemeralDestinationNonce;
    decoder >> ephemeralDestinationNonce;
    if (!ephemeralDestinationNonce)
        return std::nullopt;

    std::optional<std::optional<WebCore::RegistrableDomain>> destinationSite;
    decoder >> destinationSite;
    if (!destinationSite)
        return std::nullopt;

    return { {
        WTFMove(*data),
        WTFMove(*priority),
        WTFMove(*wasSent),
        WTFMove(*sourceRegistrableDomain),
        WTFMove(*ephemeralDestinationNonce),
        WTFMove(*destinationSite),
        // destinationUnlinkableToken and destinationSecretToken are not serialized.
    } };
}

void Coder<WebCore::RegistrableDomain, void>::encode(Encoder& encoder, const WebCore::RegistrableDomain& instance)
{
    encoder << instance.string();
}

std::optional<WebCore::RegistrableDomain> Coder<WebCore::RegistrableDomain, void>::decode(Decoder& decoder)
{
    std::optional<String> host;
    decoder >> host;
    if (!host)
        return std::nullopt;

    return { WebCore::RegistrableDomain::fromRawString(WTFMove(*host)) };
}

}
