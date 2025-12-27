#include "metrics.hpp"

static size_t cache = 0, nodeSave = 0, nodeLoad = 0;

void incrementCacheUsage() {
	++cache;
}
void incrementNodeLoadUsage() {
	++nodeLoad;
}
void incrementNodeSaveUsage() {
	++nodeSave;
}

size_t cacheUsage() {
	return cache;
}
size_t nodeLoadUsage() {
	return nodeLoad;
}
size_t nodeSaveUsage(){
	return nodeSave;
}

void resetMetrics() {
	cache = nodeSave = nodeLoad = 0;
}

