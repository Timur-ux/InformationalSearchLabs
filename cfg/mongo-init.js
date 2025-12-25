let res = [
	db.createCollection("ParsedDocuments"),
	db.createCollection("Counters"),
	db.Counters.insertOne({"_id" : "documentID", "value": 0})
];

print(res)
