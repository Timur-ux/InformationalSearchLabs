#!/usr/bin/env bash

/opt/venv/bin/scrapy crawl default_spider -s JOBDIR=/storage/crawls/default_spider-1 -o /storage/crawler.json
