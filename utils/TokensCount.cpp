#include "bplustree/INodeCache.hpp"
#include "bplustree/NodeManager/FileBased.hpp"
#include "bplustree/fwd.hpp"
#include "factory/BPlusTreeFactory.hpp"
#include <cstdint>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

using namespace std::filesystem;
using namespace IR::bplustree;
int main(int argc, const char *argw[]) {
  if (argc < 3)
    throw std::invalid_argument(
        std::format("Usage: {} <path to storage> <storage name>", argw[0]));
  path storagePath = argw[1];
  storagePath /= argw[2];

  using NodeManager = FileBasedNodeManager<std::uint32_t, std::uint32_t,
                                           SameKeyOrdering::AsInserted>;
  using Cache =
      INodeCache<std::uint32_t, std::uint32_t, SameKeyOrdering::AsInserted>;

  if (!exists(storagePath))
    throw std::runtime_error("Requested storage path not exists");
  NodeManager::Schema schema;
	bool schemaLoaded = false;

  if (exists(storagePath / "schema")) {
    std::cout << "Found storage schema, loading it\n";
    FILE *file = fopen((storagePath / "schema").c_str(), "rb");
    if (!file)
      std::cerr << "Can't open storage schema file, skip schema loading\n";
    else {
			schemaLoaded = true;
      if (fread(&schema, sizeof(NodeManager::Schema), 1, file) != 1)
        std::cerr
            << "Can't read storage schema properly, skip schema loading\n", schemaLoaded = false;

      fclose(file);
	
    }
  }

  std::shared_ptr<NodeManager> nodeManager;
  if (schemaLoaded)
    nodeManager = std::make_shared<NodeManager>(
        storagePath, schema, std::shared_ptr<Cache>(nullptr));
  else
    nodeManager = std::make_shared<NodeManager>(storagePath, nullptr);

	for(const auto & entry : directory_iterator{storagePath}) {
		auto filename = entry.path().filename();
		if(filename == "schema") 
			continue;

		long id = -1;
		try {
		 id = std::stol(filename);
		} catch(std::exception & e) {
			std::cerr << "Error while converting to long following filename: " << filename << "; skipping...\n";
			continue;
		}

		if(id == -1) {
			std::cerr << "Convertion to long for failname: " << filename << " failed; skipping...\n";
			continue;
		}

		auto node = nodeManager->load(id);
		if(!node.isLeaf) {
			std::cerr << "Skip not leaf node: " << node.id << '\n';
			continue;
		}

		for(size_t i = 0; i < node.keys.size(); ++i) 
			std::cout << node.keys.at(i).first << ' '  <<  node.values.at(i) << '\n';
	}

  return 0;
}
