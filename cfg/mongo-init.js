db.createCollection("ParsedDocuments")
db.createCollection("Counters")
db.Counters.insertOne({"_id" : "documentID", "value": 0})
