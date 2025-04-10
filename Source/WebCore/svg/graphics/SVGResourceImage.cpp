/*
 * Copyright (C) 2023 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "config.h"
#include "SVGResourceImage.h"

#include "LegacyRenderSVGResourceMasker.h"
#include "ReferencedSVGResources.h"
#include "RenderSVGResourceMasker.h"

namespace WebCore {

Ref<SVGResourceImage> SVGResourceImage::create(RenderSVGResourceContainer& renderResource, const Style::URL& resourceURL)
{
    return adoptRef(*new SVGResourceImage(renderResource, resourceURL));
}

SVGResourceImage::SVGResourceImage(RenderSVGResourceContainer& renderResource, const Style::URL& resourceURL)
    : m_renderResource(renderResource)
    , m_resourceURL(resourceURL)
{
}

Ref<SVGResourceImage> SVGResourceImage::create(LegacyRenderSVGResourceContainer& renderResource, const Style::URL& resourceURL)
{
    return adoptRef(*new SVGResourceImage(renderResource, resourceURL));
}

SVGResourceImage::SVGResourceImage(LegacyRenderSVGResourceContainer& renderResource, const Style::URL& resourceURL)
    : m_legacyRenderResource(renderResource)
    , m_resourceURL(resourceURL)
{
}

ImageDrawResult SVGResourceImage::draw(GraphicsContext& context, const FloatRect& destinationRect, const FloatRect& sourceRect, ImagePaintingOptions options)
{
    if (CheckedPtr masker = dynamicDowncast<RenderSVGResourceMasker>(m_renderResource.get())) {
        if (masker->drawContentIntoContext(context, destinationRect, sourceRect, options))
            return ImageDrawResult::DidDraw;
    }
    if (CheckedPtr masker = dynamicDowncast<LegacyRenderSVGResourceMasker>(m_legacyRenderResource.get())) {
        if (masker->drawContentIntoContext(context, destinationRect, sourceRect, options))
            return ImageDrawResult::DidDraw;
    }

    return ImageDrawResult::DidNothing;
}

void SVGResourceImage::drawPattern(GraphicsContext& context, const FloatRect& destinationRect, const FloatRect& sourceRect, const AffineTransform& patternTransform, const FloatPoint& phase, const FloatSize& spacing, ImagePaintingOptions options)
{
    RefPtr imageBuffer = context.createImageBuffer(size());
    if (!imageBuffer)
        return;

    // Fill with the SVG resource.
    GraphicsContext& graphicsContext = imageBuffer->context();
    graphicsContext.drawImage(*this, FloatPoint());

    // Tile the image buffer into the context.
    context.drawPattern(*imageBuffer, destinationRect, sourceRect, patternTransform, phase, spacing, options);
}

void SVGResourceImage::dump(WTF::TextStream& ts) const
{
    GeneratedImage::dump(ts);
    ts << m_resourceURL;
}

} // namespace WebCore
