FROM python:3.12-slim AS builder

RUN python -m venv /opt/venv

COPY ./requirements.txt .

RUN /opt/venv/bin/pip install -r ./requirements.txt

FROM python:3.12-slim 

COPY . .
COPY --from=builder /opt/venv /opt/venv/
ENV PATH="/opt/venv/bin:$PATH"

ENTRYPOINT [ "./main.sh" ]
