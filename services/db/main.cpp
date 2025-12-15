#include "BPlusTree.hpp"
#include "handlers/insert.hpp"
#include "factory/BPlusTreeFactory.hpp"
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/components/run.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/utils/resources.hpp>
#include "embedded/include/generated/static_config.yaml.hpp"

using namespace userver;

int main(int argc, const char *argw[]) {
  // const char *storagePath = getenv("STORAGE_PATH");
  // if (!storagePath)
  //   throw std::runtime_error("environment variable [STORAGE PATH] not set");
  //
  // auto forwardDB =
  //     IR::bplustree::FileBasedBPlusTreeFactory<
  //         uint32_t, uint32_t, IR::bplustree::SameKeyOrdering::AsInserted>(
  //         1024, storagePath, "forward")
  //         .createTree();
  //
  // auto backwarddDB =
  //     IR::bplustree::FileBasedBPlusTreeFactory<
  //         uint32_t, uint32_t, IR::bplustree::SameKeyOrdering::Increase>(
  //         1024, storagePath, "backward")
  //         .createTree();

  auto componentsList = components::MinimalServerComponentList()
                            .Append<dbService::InsertHandler>();

	auto config = components::InMemoryConfig{utils::FindResource(CONFIG_NAME)};

  return utils::DaemonMain(config, componentsList);
}
