// Copyright (c) 2024 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache Software License 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0, or the MIT license
// which is available at https://opensource.org/licenses/MIT.
//
// SPDX-License-Identifier: Apache-2.0 OR MIT

#include "iox/duration.hpp"
#include "iox2/log.hpp"
#include "iox2/node.hpp"
#include "iox2/service_name.hpp"
#include "iox2/service_type.hpp"

#include <iostream>

constexpr iox::units::Duration CYCLE_TIME = iox::units::Duration::fromSeconds(1);

auto main() -> int {
    using namespace iox2;
    set_log_level_from_env_or(LogLevel::Info);
    auto node = NodeBuilder().create<ServiceType::Ipc>().expect("successful node creation");

    auto service = node.service_builder(ServiceName::create("MyEventName").expect("valid service name"))
                       .event()
                       .open_or_create()
                       .expect("successful service creation/opening");

    auto listener = service.listener_builder().create().expect("successful listener creation");

    while (node.wait(iox::units::Duration::zero()).has_value()) {
        listener.timed_wait_one(CYCLE_TIME).and_then([](auto maybe_event_id) {
            maybe_event_id.and_then(
                [](auto event_id) { std::cout << "event was triggered with id: " << event_id << std::endl; });
        });
    }

    std::cout << "exit" << std::endl;

    return 0;
}
