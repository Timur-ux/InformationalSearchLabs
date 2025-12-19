# search service

Сервис обеспечивает поиск страниц

## Реализованные типы поиска

- [ ] Булевский поиск
- [ ] Цитатный поиск


## Схема запросов

1. GET /search/boolean?text={requestText}

requestText -- текст запроса

Тело ответа:

```js
[
	{
		"title" : string,
		"url" : string
	}, ...
]
```

### Примечание

Булевский поиск будет пропускать токены, которые не встречались ни в одном документе.

2. GET /search/quote?text={requestText}

requestText -- текст запроса

Тело ответа:

```js
[
	{
		"title" : string,
		"url" : string
	}, ...
]
```
