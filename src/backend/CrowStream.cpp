/* CrowStream.cpp
 *
 * Copyright 2025 Anivice Ives
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "nlohmann/json.hpp"
#include "instance.h"
#include "helper/log.h"
#include "CrowRegister.h"

using namespace std::literals;
using json = nlohmann::json;

void CrowIntAlertSSE()
{
    // Define a route that streams data
    CROW_WEBSOCKET_ROUTE(backend_instance, "/stream")
        .onopen([&](crow::websocket::connection &conn) {
            CROW_LOG_INFO << "New websocket connection from " << conn.get_remote_ip();
        })

        .onclose([&](crow::websocket::connection &, const std::string &reason, short unsigned int) {
            CROW_LOG_INFO << "Websocket connection closed: " << reason;
        })

        .onmessage([&](crow::websocket::connection &conn, const std::string & request, const bool is_binary)
        {
            auto send_data = [&](const std::string &data)
            {
                if (is_binary) {
                    conn.send_binary(data);
                } else {
                    conn.send_text(data);
                }
            };

            json response;
            try {
                json data = json::parse(request);
                const std::string operation = data["Request"];
                const std::string path = data["Path"];
                debug_log("WebSocket request: " + operation);
                if (operation == "read") {
                }
                else if (operation == "write") {
                } else {
                    throw std::invalid_argument("Unknown operation: " + operation);
                }
            } catch (const std::exception &e) {
                CROW_LOG_WARNING << "JSON parse error: " << e.what() << "\n";
                response["Result"] = "Error";
                response["Error"] = e.what();
                send_data(response.dump());
            }
        });
}
