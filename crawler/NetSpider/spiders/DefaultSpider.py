import bs4
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
        mongoAddr: str | None = getenv("MONGO_ADDR")
        if mongoAddr is None:
            raise RuntimeError("Env variable [MONGO_ADDR] not set")
        self.mongoClient = MongoClient(mongoAddr)
        self.db = self.mongoClient["index"]
        self.collection = self.db["ParsedDocuments"]
        self.counters = self.db["Counters"]

        indexerAddr: str | None = getenv("INDEXER_SERVICE_ADDR")
        if indexerAddr is None:
            raise RuntimeError("Env variable [INDEXER_SERVICE_ADDR] not set")
        self.indexerAddr: str = indexerAddr

    async def parse(self, response):
        normalizedUrl = canonicalize_url(response.url)
        title = response.css("title::text").get()
        soup = bs(response.body, 'html.parser')
        soup = self.clean_soup(soup)

        content = re.sub(r"\s{2,}", " ", str(soup))
        timestamp = datetime.now()

        oldCounter = self.counters._find_and_modify(
            filter={"_id": "documentID"}, projection=None, sort=None, update={"$inc": {"value": 1}})
        self.collection.insert_one(
                {"_id": oldCounter["value"], "url": normalizedUrl, "raw": content, "title": title, "timestamp": timestamp, "indexed": False})

        for link in response.xpath("//a/@href").getall():
            if not link.startswith("http"):
                continue
            yield response.follow(link, self.parse)

    def indexer_parse(self, response):
        print("Indexer response content:", response.text)

    def clean_soup(self, soup: bs4.element.Tag) -> bs4.element.Tag:
        for s in soup.select("script"):
            s.extract()
        for s in soup.select("style"):
            s.extract()

        return soup
