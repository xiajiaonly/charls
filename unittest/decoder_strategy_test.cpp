// Copyright (c) Team CharLS. All rights reserved. See the accompanying "LICENSE.md" for licensed use.

#include "pch.h"

#include "../src/decoder_strategy.h"

#include "encoder_strategy_tester.h"

using std::unique_ptr;
using Microsoft::VisualStudio::CppUnitTestFramework::Assert;

class DecoderStrategyTester final : public charls::DecoderStrategy
{
public:
    DecoderStrategyTester(const JlsParameters& params, uint8_t* destination, size_t nOutBufLen) :
        DecoderStrategy(params)
    {
        ByteStreamInfo stream{nullptr, destination, nOutBufLen};
        Init(stream);
    }

    void SetPresets(const JpegLSPresetCodingParameters& /*presets*/) noexcept(false) override
    {
    }

    unique_ptr<charls::ProcessLine> CreateProcess(ByteStreamInfo /*rawStreamInfo*/) noexcept(false) override
    {
        return nullptr;
    }

    void DecodeScan(unique_ptr<charls::ProcessLine> /*outputData*/, const JlsRect& /*size*/, ByteStreamInfo& /*compressedData*/) noexcept(false) override
    {
    }

    int32_t Read(int32_t length)
    {
        return ReadLongValue(length);
    }

    void Finish()
    {
        EndScan();
    }
};


namespace CharLSUnitTest
{
    TEST_CLASS(DecoderStrategyTest)
    {
    public:
        TEST_METHOD(DecodeEncodedFFPattern)
        {
            const struct
            {
                int32_t val;
                int bits;
            } inData[5] = { { 0x00, 24 },{ 0xFF, 8 },{ 0xFFFF, 16 },{ 0xFFFF, 16 },{ 0x12345678, 31 } };

            uint8_t encBuf[100];
            const JlsParameters params = { 0 };

            EncoderStrategyTester encoder(params);

            ByteStreamInfo stream;
            stream.rawStream = nullptr;
            stream.rawData = encBuf;
            stream.count = sizeof(encBuf);
            encoder.InitForward(stream);

            for (int i = 0; i < sizeof(inData) / sizeof(inData[0]); i++)
            {
                encoder.AppendToBitStreamForward(inData[i].val, inData[i].bits);
            }
            encoder.EndScanForward();
            // Note: Correct encoding is tested in EncoderStrategyTest::AppendToBitStreamFFPattern.

            const auto length = encoder.GetLengthForward();
            DecoderStrategyTester dec(params, encBuf, length);
            for (auto i = 0; i < sizeof(inData) / sizeof(inData[0]); i++)
            {
                const auto actual = dec.Read(inData[i].bits);
                Assert::AreEqual(inData[i].val, actual);
            }
        }
    };
}
