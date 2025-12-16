from typing import Any
from typing_extensions import List
import pytest
import json
from aiohttp.client import ClientSession

async def insert(service_client: ClientSession, id: int, tokens: List[int]):
    response = await service_client.post("/insert", json={
        "id" : id,
        "tokens" : tokens
        })

    assert response.status == 200
    assert 'application/json' in response.headers["Content-Type"]
    body = response.json()
    assert 'message' in body
    assert body["message"] == "success"

async def get_documents(service_client: ClientSession, tokens_id: List[int]) -> List[Any]:
    response = await service_client.get("/document", params={"id": tokens_id})
    assert response.status == 200
    assert 'application/json' in response.headers["Content-Type"]
    body = response.json()
    assert len(body) == len(tokens_id)
    return body

async def get_tokens(service_client: ClientSession, document_ids: List[int]) -> List[Any]:
    response = await service_client.get("/token", params={"id": document_ids})
    assert response.status == 200
    assert 'application/json' in response.headers["Content-Type"]
    body = response.json()
    assert len(body) == len(document_ids)
    return body
    
    
async def test_insert(service_client: ClientSession):
    await insert(service_client, 0, [0])

async def test_document(service_client):
    await insert(service_client, 1, [1, 2 ,3])
    await insert(service_client, 2, [2, 3 ,4])
    await insert(service_client, 3, [3, 4 ,5])

    values = await get_documents(service_client, [1, 2, 3])

    assert "tokenId" in values[0] and values[0]["tokenId"] == 1 and values[0]["documentIds"] == [1]
    assert "tokenId" in values[1] and values[1]["tokenId"] == 2 and values[1]["documentIds"] == [1, 2]
    assert "tokenId" in values[2] and values[2]["tokenId"] == 3 and values[2]["documentIds"] == [1, 2, 3]

async def test_tokens(service_client):
    await insert(service_client, 4, [6, 7 ,8])
    await insert(service_client, 5, [7, 8 ,9])
    await insert(service_client, 6, [8, 9, 10])

    values = await get_tokens(service_client, [4, 5, 6])

    assert "documentId" in values[0] and values[0]["documentId"] == 4 and values[0]["tokenIds"] == [6, 7 ,8]
    assert "documentId" in values[1] and values[1]["documentId"] == 5 and values[1]["tokenIds"] == [7, 8 ,9]
    assert "documentId" in values[2] and values[2]["documentId"] == 6 and values[2]["tokenIds"] == [8, 9, 10]

