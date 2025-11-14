#!/usr/bin/env bash

./venv/bin/scrapy crawl default_spider -s JOBDIR=./crawls/default_spider-1 -o ./out.json
