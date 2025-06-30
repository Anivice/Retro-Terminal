#include "instance.h"
#include "CrowRegister.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

void CrowPing()
{
    CROW_ROUTE(backend_instance, "/version").methods(crow::HTTPMethod::GET)([]()
    {
        json response;
        response["Backend"] = BACKEND_VERSION;
        response["Core"] = CORE_VERSION;
        return crow::response{response.dump()};
    });
}
