// Copyright (c) Team CharLS. All rights reserved. See the accompanying "LICENSE.md" for licensed use.

#include <charls/charls.h>
#include <charls/jpegls_error.h>

#include "jpeg_stream_reader.h"
#include "util.h"

#include <memory>
#include <new>

using namespace charls;

struct charls_jpegls_decoder
{
    void source_buffer(const void* buffer, size_t size)
    {
        if (reader_)
            throw jpegls_error{jpegls_errc::invalid_argument}; // TODO: invalid-state: or something like it.

        source_buffer_ = buffer;
        size_ = size;
    }

    void read_header()
    {
        if (reader_)
            throw jpegls_error{jpegls_errc::invalid_argument}; // TODO: invalid-state: or something like it.

        ByteStreamInfo source{FromByteArrayConst(source_buffer_, size_)};
        reader_ = std::make_unique<JpegStreamReader>(source);
        reader_->ReadHeader();
        reader_->ReadStartOfScan(true);
    }

    void decode_to_buffer(void* buffer, size_t size)
    {
        if (!reader_)
            throw jpegls_error{jpegls_errc::invalid_argument}; // TODO: invalid-state: or something like it.

        const ByteStreamInfo destination = FromByteArray(buffer, size);
        reader_->Read(destination);
    }

    bool parse_tables;
    bool parse_comments;

private:
    std::unique_ptr<JpegStreamReader> reader_;
    const void* source_buffer_;
    size_t size_;
};

extern "C"
{
    charls_jpegls_decoder* CHARLS_API_CALLING_CONVENTION
    charls_jpegls_decoder_create()
    {
        MSVC_WARNING_SUPPRESS(26402 26409) // don't use new and delete + scoped object and move
        return new (std::nothrow) charls_jpegls_decoder;
        MSVC_WARNING_UNSUPPRESS()
    }

    void CHARLS_API_CALLING_CONVENTION
    charls_jpegls_decoder_destroy(charls_jpegls_decoder* decoder)
    {
        MSVC_WARNING_SUPPRESS(26401 26409) // don't use new and delete + non-owner.
        delete decoder;
        MSVC_WARNING_UNSUPPRESS()
    }

    jpegls_errc CHARLS_API_CALLING_CONVENTION
    charls_jpegls_decoder_source_buffer(charls_jpegls_decoder* decoder, const void* buffer, size_t size)
    {
        try
        {
            decoder->source_buffer(buffer, size);
            return jpegls_errc::success;
        }
        catch (...)
        {
            return to_jpegls_errc();
        }
    }

    charls_jpegls_errc CHARLS_API_CALLING_CONVENTION
    charls_jpegls_decoder_read_spiff_header(charls_jpegls_decoder* /*decoder*/, void* /*header*/)
    {
        try
        {
            return jpegls_errc::unexpected_failure; // TODO
        }
        catch (...)
        {
            return to_jpegls_errc();
        }
    }

    jpegls_errc CHARLS_API_CALLING_CONVENTION
    charls_jpegls_decoder_read_header(charls_jpegls_decoder* decoder)
    {
        try
        {
            decoder->read_header();
            return jpegls_errc::success;
        }
        catch (...)
        {
            return to_jpegls_errc();
        }
    }

    jpegls_errc CHARLS_API_CALLING_CONVENTION
    charls_jpegls_decoder_decode_to_buffer(charls_jpegls_decoder* decoder, void* buffer, size_t size)
    {
        try
        {
            decoder->decode_to_buffer(buffer, size);
            return jpegls_errc::success;
        }
        catch (...)
        {
            return to_jpegls_errc();
        }
    }
}