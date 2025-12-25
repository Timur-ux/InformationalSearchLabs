#include "components/index.hpp"
#include <cstdlib>
#include <cstring>
#include <userver/clients/http/component.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/components/run.hpp>
#include <userver/storages/mongo/component.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/utils/resources.hpp>
#include "embedded/include/generated/static_config.yaml.hpp"

using namespace userver;
using namespace SERVICE_NAMESPACE;

int main(int argc, const char * argw[]) {
  auto componentsList = components::MinimalServerComponentList()
														.Append<clients::dns::Component>()
														.Append<components::Mongo>("mongo-index")
														.Append<components::HttpClient>("index-http-client")
														.Append<Indexer>();

	bool useInMemoryConfig = true;
	for(int i = 1; i < argc && useInMemoryConfig; ++i) 
		if(strcmp(argw[i], "--config") == 0) 
			useInMemoryConfig = false;
		
	auto config = components::InMemoryConfig{utils::FindResource(CONFIG_NAME)};

	if(useInMemoryConfig) 
		return utils::DaemonMain(config, componentsList);
	else
		return utils::DaemonMain(argc, argw, componentsList);
}
