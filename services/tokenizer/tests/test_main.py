from typing import Any
from typing_extensions import List
import pytest
from aiohttp.client import ClientSession

text = "Some text with english и русскими буквами"

async def tokenize(service_client: ClientSession, text: str) -> List[int]:
    response = await service_client.post("/tokenize", json=text)

    assert response.status == 200
    assert 'application/json' in response.headers["Content-Type"]
    body = response.json()
    assert type(body) is list
    for token in body:
        assert type(token) is int

    return body

async def test_tokenize(service_client: ClientSession):
    await tokenize(service_client, text)

async def test_detokenize(service_client: ClientSession):
    tokens = await tokenize(service_client, text)
    response = await service_client.post("/detokenize", json=tokens)

    assert response.status == 200
    assert 'application/json' in response.headers["Content-Type"]

    body = response.json()
    assert type(body) is str
    assert body.strip() == text.lower()
    


    

