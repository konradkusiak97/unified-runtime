// Copyright (C) 2023 Intel Corporation
// Part of the Unified-Runtime Project, under the Apache License v2.0 with LLVM Exceptions.
// See LICENSE.TXT
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "fixtures.h"

using urEventGetProfilingInfoTest =
    uur::event::urEventTestWithParam<ur_profiling_info_t>;

TEST_P(urEventGetProfilingInfoTest, Success) {

    ur_profiling_info_t info_type = getParam();
    size_t size;
    ASSERT_SUCCESS(
        urEventGetProfilingInfo(event, info_type, 0, nullptr, &size));
    ASSERT_EQ(size, 8);

    std::vector<uint8_t> data(size);
    ASSERT_SUCCESS(
        urEventGetProfilingInfo(event, info_type, size, data.data(), nullptr));

    if (sizeof(size_t) == size) {
        auto returned_value = reinterpret_cast<size_t *>(data.data());
        ASSERT_NE(*returned_value, 0);
    }
}

UUR_TEST_SUITE_P(urEventGetProfilingInfoTest,
                 ::testing::Values(UR_PROFILING_INFO_COMMAND_QUEUED,
                                   UR_PROFILING_INFO_COMMAND_SUBMIT,
                                   UR_PROFILING_INFO_COMMAND_START,
                                   UR_PROFILING_INFO_COMMAND_END,
                                   UR_PROFILING_INFO_COMMAND_COMPLETE),
                 uur::deviceTestWithParamPrinter<ur_profiling_info_t>);

using urEventGetProfilingInfoWithTimingComparisonTest = uur::event::urEventTest;

TEST_P(urEventGetProfilingInfoWithTimingComparisonTest, Success) {
    uint8_t size = 8;

    std::vector<uint8_t> queued_data(size);
    ASSERT_SUCCESS(urEventGetProfilingInfo(event,
                                           UR_PROFILING_INFO_COMMAND_QUEUED,
                                           size, queued_data.data(), nullptr));
    auto queued_timing = reinterpret_cast<size_t *>(queued_data.data());
    ASSERT_NE(*queued_timing, 0);

    std::vector<uint8_t> submit_data(size);
    ASSERT_SUCCESS(urEventGetProfilingInfo(event,
                                           UR_PROFILING_INFO_COMMAND_SUBMIT,
                                           size, submit_data.data(), nullptr));
    auto submit_timing = reinterpret_cast<size_t *>(submit_data.data());
    ASSERT_NE(*submit_timing, 0);

    std::vector<uint8_t> start_data(size);
    ASSERT_SUCCESS(urEventGetProfilingInfo(event,
                                           UR_PROFILING_INFO_COMMAND_START,
                                           size, start_data.data(), nullptr));
    auto start_timing = reinterpret_cast<size_t *>(start_data.data());
    ASSERT_NE(*start_timing, 0);

    std::vector<uint8_t> end_data(size);
    ASSERT_SUCCESS(urEventGetProfilingInfo(event, UR_PROFILING_INFO_COMMAND_END,
                                           size, end_data.data(), nullptr));
    auto end_timing = reinterpret_cast<size_t *>(end_data.data());
    ASSERT_NE(*end_timing, 0);

    std::vector<uint8_t> complete_data(size);
    ASSERT_SUCCESS(
        urEventGetProfilingInfo(event, UR_PROFILING_INFO_COMMAND_COMPLETE, size,
                                complete_data.data(), nullptr));
    auto complete_timing = reinterpret_cast<size_t *>(complete_data.data());
    ASSERT_NE(*complete_timing, 0);

    ASSERT_LE(*queued_timing, *submit_timing);
    ASSERT_LT(*submit_timing, *start_timing);
    ASSERT_LT(*start_timing, *end_timing);
    ASSERT_LE(*end_timing, *complete_timing);
}

UUR_INSTANTIATE_DEVICE_TEST_SUITE_P(
    urEventGetProfilingInfoWithTimingComparisonTest);

using urEventGetProfilingInfoNegativeTest = uur::event::urEventTest;

TEST_P(urEventGetProfilingInfoNegativeTest, InvalidNullHandle) {
    ur_profiling_info_t info_type = UR_PROFILING_INFO_COMMAND_QUEUED;
    size_t size;
    ASSERT_SUCCESS(
        urEventGetProfilingInfo(event, info_type, 0, nullptr, &size));
    ASSERT_NE(size, 0);
    std::vector<uint8_t> data(size);

    /* Invalid hEvent */
    ASSERT_EQ_RESULT(
        urEventGetProfilingInfo(nullptr, info_type, 0, nullptr, &size),
        UR_RESULT_ERROR_INVALID_NULL_HANDLE);
}

TEST_P(urEventGetProfilingInfoNegativeTest, InvalidEnumeration) {
    size_t size;
    ASSERT_EQ_RESULT(urEventGetProfilingInfo(event,
                                             UR_PROFILING_INFO_FORCE_UINT32, 0,
                                             nullptr, &size),
                     UR_RESULT_ERROR_INVALID_ENUMERATION);
}

TEST_P(urEventGetProfilingInfoNegativeTest, InvalidValue) {
    ur_profiling_info_t info_type = UR_PROFILING_INFO_COMMAND_QUEUED;
    size_t size;
    ASSERT_SUCCESS(
        urEventGetProfilingInfo(event, info_type, 0, nullptr, &size));
    ASSERT_NE(size, 0);
    std::vector<uint8_t> data(size);

    /* Invalid propSize */
    ASSERT_EQ_RESULT(
        urEventGetProfilingInfo(event, info_type, 0, data.data(), nullptr),
        UR_RESULT_ERROR_INVALID_VALUE);
}

UUR_INSTANTIATE_DEVICE_TEST_SUITE_P(urEventGetProfilingInfoNegativeTest);
