#!/usr/bin/env bash

/venv/bin/scrapy crawl default_spider -s JOBDIR=/storage/crawls/default_spider-1 -o /storage/out.json
