//
// Created by 吴子庶 on 2021/10/6.
//

#pragma once


namespace Envoy {
    namespace Http {

        enum MetricEvent { PASS = 0, BLOCK = 1, EXCEPTION = 2, SUCCESS = 3, RT = 4 };

    } // namespace Http
} // namespace Envoy