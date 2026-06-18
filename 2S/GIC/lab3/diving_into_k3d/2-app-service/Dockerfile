# Diving into docker

FROM python:3.10-alpine

LABEL version="1.0"

RUN mkdir /app

WORKDIR /app

COPY requirements.txt requirements.txt

RUN pip install -r requirements.txt

COPY app.py app.py

EXPOSE 8080/tcp

ENTRYPOINT ["python3", "app.py"]
