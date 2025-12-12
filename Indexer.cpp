#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json-fwd.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/view_or_value-fwd.hpp>
#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <string_view>
#include "base64.hpp"

using bsoncxx::builder::basic::make_document;
using bsoncxx::builder::basic::kvp;

int main(int argc, char * argw[]) {
	mongocxx::instance instance;
	mongocxx::client client(mongocxx::v_noabi::uri("mongodb://localhost:27017"));
	mongocxx::database db = client.database("test");
	auto collection = db.collection("ParsedDocuments");

	std::cout << "Documents in collection: " << collection.count_documents(make_document()) << '\n';
	auto cursor = collection.find(make_document());
	size_t i = 0;
	for ( auto docs = cursor.begin(); docs != cursor.end(); ++docs) {
		for ( auto keys = docs->begin(); keys != docs->end(); ++keys) {
			if(keys->key() == "url") 
				std::cout << "url: " << keys->get_string().value << '\n';

			if(keys->key() == "raw") {
				auto base64Raw = keys->get_binary().bytes;
				std::string_view html = std::string_view((const char*)base64Raw);
				std::cout << html << '\n';
			}
		}
	
		if(i++ > 10) 
			break;
	}
	return 0;
}
