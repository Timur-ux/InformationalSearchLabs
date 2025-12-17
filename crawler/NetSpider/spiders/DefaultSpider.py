import scrapy
from scrapy.http import HtmlResponse
from scrapy.utils.url import canonicalize_url
from settings import ALLOWED_DOMAINS, INITIAL_URLS
from pymongo import MongoClient
from datetime import datetime
from bs4 import BeautifulSoup as bs
import re



class DefaultSpider(scrapy.Spider):
    name = "default_spider"
    start_urls = INITIAL_URLS
    allowed_domains = ALLOWED_DOMAINS

    def __init__(self, *args, **kwargs) -> None:
        super().__init__(self.name, *args, **kwargs)
        self.parsed = 0
        self.mongoClient = MongoClient("mongodb://localhost:27017/")
        self.db = self.mongoClient["index"]
        self.collection = self.db["ParsedDocuments"]
        self.counters = self.db["Counters"]

    def parse(self, response):
        normalizedUrl = canonicalize_url(response.url)
        title = response.css("title::text").get()
        soup = bs(response.body, 'html.parser')
        content = re.sub(r"\s{2,}", " ", soup.get_text(separator=' '))
        timestamp = datetime.now()
        oldCounter = self.counters._find_and_modify(filter={"_id": "documentID"}, projection=None, sort=None, update={"$inc": {"value": 1}})
        self.collection.insert_one(
                {"_id": oldCounter["value"], "url": normalizedUrl, "raw": content, "title": title, "timestamp": timestamp})
        for link in response.xpath("//a/@href").getall():
            if not link.startswith("http"):
                continue
            # yield response.follow(link, self.parse)
