import scrapy
from scrapy.http import HtmlResponse
from scrapy.utils.url import canonicalize_url
from settings import ALLOWED_DOMAINS, INITIAL_URLS
from pymongo import MongoClient
from datetime import datetime


class DefaultSpider(scrapy.Spider):
    name = "default_spider"
    start_urls = INITIAL_URLS
    allowed_domains = ALLOWED_DOMAINS

    def __init__(self, *args, **kwargs) -> None:
        super().__init__(self.name, *args, **kwargs)
        self.parsed = 0
        self.mongoClient = MongoClient("mongodb://localhost:27017/")
        self.db = self.mongoClient["test"]
        self.collection = self.db["ParsedDocuments"]

    def parse(self, response):
        normalizedUrl = canonicalize_url(response.url)
        title = response.css("title::text").get()
        content = response.body
        timestamp = datetime.now()
        self.collection.insert_one(
            {"url": normalizedUrl, "raw": content, "title": title, "timestamp": timestamp})
        for link in response.xpath("//a/@href").getall():
            if not link.startswith("http"):
                continue
            yield response.follow(link, self.parse)
