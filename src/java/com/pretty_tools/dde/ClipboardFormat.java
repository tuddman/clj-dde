/*
 * Copyright 2013 www.pretty-tools.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.pretty_tools.dde;

/**
 * Standard clipboard formats.
 *
 * @author Alexander Kozlov (alex@pretty-tools.com)
 */
public enum ClipboardFormat
{
    CF_TEXT(1),
    CF_BITMAP(2),
    CF_METAFILEPICT(3),
    CF_SYLK(4),
    CF_DIF(5),
    CF_TIFF(6),
    CF_OEMTEXT(7),
    CF_DIB(8),
    CF_PALETTE(9),
    CF_PENDATA(10),
    CF_RIFF(11),
    CF_WAVE(12),
    CF_UNICODETEXT(13),
    CF_ENHMETAFILE(14);

    final int fmt;

    private ClipboardFormat(int fmt)
    {
        this.fmt = fmt;
    }

    public int getNativeCode()
    {
        return fmt;
    }

    public static ClipboardFormat valueOf(int fmt)
    {
        for (ClipboardFormat clipboardFormat : ClipboardFormat.values())
        {
            if (fmt == clipboardFormat.getNativeCode())
                return clipboardFormat;
        }
        return null;
    }
}
