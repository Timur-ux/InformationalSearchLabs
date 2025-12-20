import scrapy
import json
from scrapy.utils.url import canonicalize_url
from settings import ALLOWED_DOMAINS, INITIAL_URLS
from pymongo import MongoClient
from datetime import datetime
from bs4 import BeautifulSoup as bs
import re
from os import getenv
import requests

class DefaultSpider(scrapy.Spider):
    name = "default_spider"
    start_urls = INITIAL_URLS
    allowed_domains = ALLOWED_DOMAINS

    def __init__(self, *args, **kwargs) -> None:
        super().__init__(self.name, *args, **kwargs)
        self.parsed = 0
        mongoAddr: str = getenv("MONGO_ADDR")
        if mongoAddr is None:
            raise RuntimeError("Env variable [MONGO_ADDR] not set")
        self.mongoClient = MongoClient(mongoAddr)
        self.db = self.mongoClient["index"]
        self.collection = self.db["ParsedDocuments"]
        self.counters = self.db["Counters"]

        self.indexerAddr: str = getenv("INDEXER_SERVICE_ADDR")
        if self.indexerAddr is None:
            raise RuntimeError("Env variable [INDEXER_SERVICE_ADDR] not set")

    async def parse(self, response):
        normalizedUrl = canonicalize_url(response.url)
        title = response.css("title::text").get()
        soup = bs(response.body, 'html.parser')
        content = re.sub(r"\s{2,}", " ", soup.get_text(separator=' '))
        timestamp = datetime.now()
        oldCounter = self.counters._find_and_modify(
            filter={"_id": "documentID"}, projection=None, sort=None, update={"$inc": {"value": 1}})
        print("OLD COUNTER:", oldCounter)
        self.collection.insert_one(
            {"_id": oldCounter["value"], "url": normalizedUrl, "raw": content, "title": title, "timestamp": timestamp})

        self.indexer_parse(requests.post(url=f"{self.indexerAddr}/doIndex", json={"pageId": oldCounter["value"]}))
        for link in response.xpath("//a/@href").getall():
            if not link.startswith("http"):
                continue
            yield response.follow(link, self.parse)

    def indexer_parse(self, response):
        print("Indexer response content:", response.text)
