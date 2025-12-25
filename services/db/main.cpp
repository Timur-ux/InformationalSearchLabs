#include "components/DataBase.hpp"
#include "embedded/include/generated/static_config.yaml.hpp"
#include "handlers/document.hpp"
#include "handlers/insert.hpp"
#include "handlers/token.hpp"
#include <cstdlib>
#include <cstring>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/components/run.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/utils/resources.hpp>

using namespace userver;
using namespace SERVICE_NAMESPACE;

int main(int argc, const char *argw[]) {
  auto componentsList = components::MinimalServerComponentList()
                            .Append<DataBaseComponent>()
                            .Append<InsertHandler>()
                            .Append<DocumentHandler>()
                            .Append<TokenHandler>();

  bool useInMemoryConfig = true;
  for (int i = 1; i < argc && useInMemoryConfig; ++i)
    if (strcmp(argw[i], "--config") == 0)
      useInMemoryConfig = false;

  auto config = components::InMemoryConfig{utils::FindResource(CONFIG_NAME)};

  if (useInMemoryConfig)
    return utils::DaemonMain(config, componentsList);
  else
    return utils::DaemonMain(argc, argw, componentsList);
}
