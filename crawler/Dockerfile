FROM python:3.12-slim

WORKDIR /app

COPY . .

RUN python -m venv /venv && /venv/bin/python -m pip install -r /app/requirements.txt

ENTRYPOINT [ "./main.sh" ]
